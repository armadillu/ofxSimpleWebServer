//
//  ofxSimpleWebServer.cpp
//  SensorData
//
//  Created by Oriol Ferrer Mesià on 30/10/2020.
//
//

#include "ofxSimpleWebServer.h"
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPServerRequest.h>

using namespace std;
using namespace Poco::Net;


ofxSimpleWebServer::ofxSimpleWebServer(){}

ofxSimpleWebServer::~ofxSimpleWebServer() {

	if (webServer){
		try{
			webServer->stopAll();
			webServer->stop();
		}catch(std::exception & e){
			ofLogError("ofxSimpleWebServer") << "Exception trying to stop server" << e.what();
		}

		while(webServer->currentThreads() > 0){
			ofSleepMillis(5);
		}
		delete webServer;
	}
}


void ofxSimpleWebServer::setup(int port, int maxThreads){

	if(!isSetup){

		ofLogNotice("ofxSimpleWebServer") << "trying to setup at port " << port;
		this->port = port;
		this->maxThreads = maxThreads;

		//http server params
		HTTPServerParams* pParams = new HTTPServerParams;
		pParams->setKeepAlive(true);
		pParams->setMaxKeepAliveRequests(2);
		pParams->setKeepAliveTimeout(3);
		pParams->setThreadIdleTime(Poco::Timespan(2,0));
		pParams->setMaxQueued(32);
		pParams->setMaxThreads(maxThreads);

		//customize socket options
		ServerSocket * ssocket = new ServerSocket(port);
		ssocket->setLinger(true, 1);
		ssocket->setNoDelay(true);
		ssocket->setBlocking(true);
		ssocket->setReuseAddress(true);

		threadPool = new Poco::ThreadPool("ofxSimpleWebServerThrPool", 1, MIN(maxThreads, 1), 30/*idle time*/);
		webServer = new HTTPServer(new ofxSimpleWebServer::RequestHandlerFactory(&sharedData), *threadPool, *ssocket, pParams);

		ofLogNotice("ofxSimpleWebServer") << "setup on port " << port;
		isSetup = true;
	}else{
		ofLogError("ofxSimpleWebServer") << "cant setup; already setup!";
	}
}

ofFastEvent<ofxSimpleWebServer::WebRequestAction>& ofxSimpleWebServer::getRequestEvent(){
	return sharedData.eventWebRequest;
}

int ofxSimpleWebServer::getNumCurrentRequests(){
	return sharedData.numCurrentRequests;
};


string ofxSimpleWebServer::getStats(){
	string msg = "currentThreads:" + ofToString(webServer->currentThreads()) + "\n";
	msg += "maxThreads:" + ofToString(webServer->maxThreads()) + "\n";
	msg += "totalConnections:" + ofToString(webServer->totalConnections()) + "\n";
	msg += "currentConnections:" + ofToString(webServer->currentConnections()) + "\n";
	msg += "maxConcurrentConnections:" + ofToString(webServer->maxConcurrentConnections()) + "\n";
	msg += "queuedConnections:" + ofToString(webServer->queuedConnections()) + "\n";
	msg += "refusedConnections:" + ofToString(webServer->refusedConnections()) + "\n";
	return msg;
}


void ofxSimpleWebServer::startServer(){
	if(!serverRunning){
		webServer->start();
		ofLogNotice("ofxActionWebSever") << "starting server!";
	}else{
		ofLogError("ofxActionWebSever") << "can't start server, already started!";
	}
}


void ofxSimpleWebServer::stopServer(){
	if(serverRunning){
		ofLogNotice("ofxActionWebSever") << "stopping server!";
		try{
			webServer->stopAll();
			webServer->stop();
		}catch(std::exception & e){
			ofLogError("ofxSimpleWebServer") << "Exception trying to stop server" << e.what();
		}
	}else{
		ofLogError("ofxActionWebSever") << "can't stop server, not started!";
	}
}


void ofxSimpleWebServer::RequestHandler::handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp) {

	if(data->shouldLog){
		ofLogNotice("ofxSimpleWebServer") << "got web request from \"" << req.getHost() << "\" \"" << req.getURI() << "\"";
	}

	data->numCurrentRequests++;

	try{

//		if(data->shouldLog) ofLogWarning("ofxSimpleWebServer") << "Something went wrong requesting endpoint \"" << uri << "\" 404!";
//		static char buffer[10] = "404\n";
//		resp.setStatus(HTTPResponse::HTTP_NOT_FOUND);
//		resp.sendBuffer(buffer, 4);

		WebRequestAction action = {req, resp, data->server};
		ofNotifyEvent(data->eventWebRequest, action, this);

	}catch(std::exception & e){
		ofLogFatalError("ofxSimpleWebServer") << e.what();
	}
	data->numCurrentRequests--;
}


//// RequestHandler //////////////////////////////////////////////////////////////////////////////////////////////////////////////

ofxSimpleWebServer::RequestHandler::RequestHandler(SharedData * data){
	this->data = data;
}

//// RequestHandler //////////////////////////////////////////////////////////////////////////////////////////////////////////////

ofxSimpleWebServer::RequestHandlerFactory::RequestHandlerFactory(SharedData * data){
	this->data = data;
}

HTTPRequestHandler* ofxSimpleWebServer::RequestHandlerFactory::createRequestHandler(const HTTPServerRequest& request){
	return new RequestHandler(data);
}

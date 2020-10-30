#include "ofApp.h"
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/SocketAddress.h>

//--------------------------------------------------------------
void ofApp::setup(){

	ofAddListener(server.getRequestEvent(), this, &ofApp::onWebRequestAction);
	server.setup(8080, 3);
	server.startServer();

}

//--------------------------------------------------------------
void ofApp::update(){

	//transfer any pending log lines (from http thread, not thread safe) to local log
	mutex.lock();
	for(auto & l : pendingLogLines){
		log.push_back(l);
	}
	pendingLogLines.clear();
	mutex.unlock();
}

//--------------------------------------------------------------
void ofApp::draw(){

	int n = 0;
	for(auto & l : log){
		ofDrawBitmapString(l, 20, 20 + 14 * n);
		n++;
	}
}


void ofApp::onWebRequestAction(ofxSimpleWebServer::WebRequestAction & act){

	ofLogNotice() << "got Web Request from host: \"" << act.request.getHost()
	<< "\" Method: \"" << act.request.getMethod() << "\" URI: \"" << act.request.getURI() << "\"";

	string uri = act.request.getURI();

	mutex.lock();
		string logLine = "Got Request \"" + act.request.getHost() + uri + "\" from client: \"" + act.request.clientAddress().host().toString() + "\"";
		log.push_back(logLine);
	mutex.unlock();

	//example: respond with a file /////////////////////////////////////////

	if (uri == "/getFile"){
		ofFile f;
		f.open("../../src/main.cpp");
		act.response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
		act.response.set("Content-Length", ofToString(f.getSize()));
		act.response.sendFile(f.getAbsolutePath(), "text/plain");
	}else

	//example: raw response //////////////////////////////////////////////

	if (uri == "/test"){
		act.response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
		char buffer[128];
		int n = sprintf(buffer, "It's now %s", ofGetTimestampString().c_str());
		act.response.sendBuffer(buffer, n);
	}

	//example: 404 error reponse ////////////////////////////////////////

	else{
		static char buffer[32] = "Not Found 404";
		act.response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
		act.response.sendBuffer(buffer, strlen(buffer));
	}
}

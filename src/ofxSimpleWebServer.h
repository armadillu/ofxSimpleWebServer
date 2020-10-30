//
//  ofxSimpleWebServer.h
//  SensorData
//
//  Created by Oriol Ferrer Mesià on 30/10/2020.
//
//

#pragma once
#include "ofMain.h"
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>

class ofxSimpleWebServer{

public:
	
	ofxSimpleWebServer();
	~ofxSimpleWebServer();

	void setup(int port, int maxThreads);

	void startServer();
	void stopServer();

	int getNumCurrentRequests();
	string getStats();

	struct WebRequestAction{
		Poco::Net::HTTPServerRequest & request;
		Poco::Net::HTTPServerResponse & response;
		ofxSimpleWebServer * server;
	};

	ofFastEvent<ofxSimpleWebServer::WebRequestAction>& getRequestEvent();

protected:

	Poco::Net::HTTPServer * webServer = nullptr;
	Poco::ThreadPool * threadPool = nullptr;

	struct SharedData{
		int numCurrentRequests = 0;
		bool shouldLog = false;
		ofFastEvent<WebRequestAction> eventWebRequest;
		ofxSimpleWebServer * server = nullptr;
	};

	SharedData sharedData;

	bool isSetup = false;
	int numRequests = 0;
	int port = 0;
	int maxThreads;
	bool serverRunning = false;

	///////////////////////////////////////////////// Internal Server subclassing - Poco
	// all of this is just to be able to pass down params to each http request

	class RequestHandler : public Poco::Net::HTTPRequestHandler {
	public:
		RequestHandler(SharedData * data);
		virtual void handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp);
	protected:
		SharedData * data;
	};


	/////////////////////////////////////////////////

	class RequestHandlerFactory: public Poco::Net::HTTPRequestHandlerFactory{
	public:
		RequestHandlerFactory(SharedData * data);
		Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);
	protected:
		SharedData * data;
	};
};



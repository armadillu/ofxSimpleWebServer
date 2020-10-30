#pragma once

#include "ofMain.h"
#include "ofxSimpleWebServer.h"

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key){};
	void keyReleased(int key){};
	void mouseMoved(int x, int y ){};
	void mouseDragged(int x, int y, int button){};
	void mousePressed(int x, int y, int button){};
	void mouseReleased(int x, int y, int button){};
	void mouseEntered(int x, int y){};
	void mouseExited(int x, int y){};
	void windowResized(int w, int h){};
	void dragEvent(ofDragInfo dragInfo){};
	void gotMessage(ofMessage msg){};

	ofxSimpleWebServer server;
	void onWebRequestAction(ofxSimpleWebServer::WebRequestAction & action);

	//log lines coming from http requests, not thread safe!
	vector<std::string> pendingLogLines;
	std::mutex mutex;

	//consolidated log from any requests (thread safe)
	vector<std::string> log;
};

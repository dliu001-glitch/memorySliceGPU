#pragma once

#include "DataManager.h"
#include "ofMain.h"
#include "ofxGui.h"

class Screen3App : public ofBaseApp {
public:
	Screen3App();
	~Screen3App() { cleanupTBO(); }
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void keyReleased(int key) { }
	void mouseMoved(int x, int y) { }
	void mouseDragged(int x, int y, int button) { }
	void mousePressed(int x, int y, int button) { }
	void mouseReleased(int x, int y, int button) { }
	void windowResized(int w, int h) { handleWindowResize(w, h); }

private:
	// Core components
	DataManager & dataManager;
	ofEasyCam cam;
	ofShader fusionShader;
	ofFbo finalFBO;

	// TBO for mesh fusion
	GLuint screen1PositionTBO;
	GLuint screen1PositionTexture; // The texture object for TBO
	bool tboInitialized;

	// Driving mesh (we'll use Screen2's mesh as driver)
	ofVboMesh drivingMesh;
	bool hasDrivingMesh;

	// GUI controls
	ofxPanel gui;
	ofParameter<float> mixRatio;
	ofParameter<bool> enableFusion;
	ofParameter<bool> showDebugInfo;
	bool showGui;

	// Setup functions
	void setupCamera();
	void setupShader();
	void setupFBO();
	void setupGui();

	// TBO management
	void setupTBO();
	void updateScreen1TBO();
	void cleanupTBO();

	// Mesh management
	void updateDrivingMesh();

	// Rendering
	void renderFusion();
	void renderUI();

	// Utility
	string getDebugInfo();
	void handleWindowResize(int w, int h);
	void logSystemInfo();
};

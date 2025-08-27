#include "ofApp.h"

void ofApp::setup() {
	ofLogNotice() << "OpenGL Version: " << glGetString(GL_VERSION);
	ofLogNotice() << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION);
	ofLogNotice() << "OpenGL Vendor: " << glGetString(GL_VENDOR);
	ofLogNotice() << "OpenGL Renderer: " << glGetString(GL_RENDERER);
	screenApp.setup();
	screen1.setup();
	screen3.setup();
}

void ofApp::update() {
	screenApp.update();
	screen1.update();
	screen3.update();
}

void ofApp::draw() {
	screenApp.draw();
	screen1.draw();
	screen3.draw();
}

void ofApp::keyPressed(int key) {
	screenApp.keyPressed(key);
	screen1.keyPressed(key);
	screen3.keyPressed(key);
}

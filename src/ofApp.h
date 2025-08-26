// ����OF��׼��ofApp.h
#pragma once
#include "ofMain.h"
#include "screens/Screen1App.h"
#include "screens/Screen2App.h"
#include "screens/Screen3App.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	// ... ����OF��׼����

private:
	Screen2App screenApp; // ί�и��������ĻӦ��
	Screen1App screen1;
	Screen3App screen3;
};

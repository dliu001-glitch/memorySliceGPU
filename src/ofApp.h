// 保留OF标准的ofApp.h
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
	// ... 其他OF标准方法

private:
	Screen2App screenApp; // 委托给具体的屏幕应用
	Screen1App screen1;
	Screen3App screen3;
};

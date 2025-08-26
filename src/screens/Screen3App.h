#pragma once
#include "CubeMesh.h"
#include "DataManager.h"
#include "ModelLoader.h"
#include "geometryData.h"
#include "ofMain.h"
#include "ofxGui.h"

class Screen3App : public ofBaseApp {
public:
	Screen3App();

	void setup();
	void update();
	void draw();
	void keyPressed(int key);

private:
	// === 核心组件 ===
	DataManager & dataManager;
	ofEasyCam cam;
	ofFbo finalFBO;

	// === 屏幕空间融合 ===
	ofShader screenSpaceMixShader;
	ofPlanePrimitive fullScreenQuad;

	// === 位置纹理数据 ===
	ofTexture screen1PosTexture, screen1DepthTexture;
	ofTexture screen2PosTexture, screen2DepthTexture;
	bool hasValidTextures = false;

	// === GUI控制 ===
	ofxPanel gui;
	bool showGui = true;

	// 混合控制
	ofParameter<float> guiMixRatio;
	ofParameter<bool> guiEnableModel;
	ofParameter<bool> guiEnableGeometry;
	ofParameter<float> guiModelInfluence;
	ofParameter<float> guiGeometryInfluence;

	// 视觉控制
	ofParameter<bool> guiShowScreen1;
	ofParameter<bool> guiShowScreen2;
	ofParameter<ofColor> guiMixColor;

	// === 光照参数（用于重新计算光照） ===
	LightingParams lightingParams;

	// === 内部方法 ===
	void setupCamera();
	void setupShaders();
	void setupFBO();
	void setupGui();
	void setupFullScreenQuad();

	void updatePositionTextures();
	void renderScreenSpaceMix();
	void handleWindowResize(int w, int h);

	ofVec3f calculateLightPosition();
	string getDebugInfo();
	void renderUI();
	void resetAllParameters();
	void logSystemInfo();

	// === 状态变量 ===
	float elapsedTime = 0.0f;
};

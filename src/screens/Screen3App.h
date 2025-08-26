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
	// === ������� ===
	DataManager & dataManager;
	ofEasyCam cam;
	ofFbo finalFBO;

	// === ��Ļ�ռ��ں� ===
	ofShader screenSpaceMixShader;
	ofPlanePrimitive fullScreenQuad;

	// === λ���������� ===
	ofTexture screen1PosTexture, screen1DepthTexture;
	ofTexture screen2PosTexture, screen2DepthTexture;
	bool hasValidTextures = false;

	// === GUI���� ===
	ofxPanel gui;
	bool showGui = true;

	// ��Ͽ���
	ofParameter<float> guiMixRatio;
	ofParameter<bool> guiEnableModel;
	ofParameter<bool> guiEnableGeometry;
	ofParameter<float> guiModelInfluence;
	ofParameter<float> guiGeometryInfluence;

	// �Ӿ�����
	ofParameter<bool> guiShowScreen1;
	ofParameter<bool> guiShowScreen2;
	ofParameter<ofColor> guiMixColor;

	// === ���ղ������������¼�����գ� ===
	LightingParams lightingParams;

	// === �ڲ����� ===
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

	// === ״̬���� ===
	float elapsedTime = 0.0f;
};

#pragma once
#include "core/DataManager.h"
#include "geometry/ModelLoader.h"
#include "ofMain.h"
#include "ofxGui.h"
#include "shared/CommonStructs.h"
#include "shared/GeometryData.h"

class Screen1App : public ofBaseApp {
public:
	Screen1App();
	~Screen1App() = default;

	void setup() override;
	void update() override;
	void draw() override;
	void keyPressed(int key) override;
	void dragEvent(ofDragInfo dragInfo) override;
	ofFbo & getPositionFBO() { return positionFBO; }

private:
	// === 核心组件 ===
	ModelLoader modelLoader;
	ofEasyCam cam;
	ofFbo fbo;
	ofShader modelShader;
	DataManager & dataManager;

	// === 模型相关 ===
	ofVboMesh loadedModel;
	bool isModelLoaded = false;
	string currentModelPath = "";

	// === 动画参数 ===
	float elapsedTime = 0.0f;
	float rotationSpeed = 30.0f;
	float currentRotationY = 0.0f;

	// === 渲染配置 ===
	ofVec3f modelPosition = ofVec3f(0, 0, 0);
	ofVec3f modelScale = ofVec3f(1, 1, 1);
	LightingParams lightingParams;

	// === GUI相关 ===
	ofxPanel gui;
	bool showGui = true;

	ofParameter<bool> guiAutoRotation;
	ofParameter<float> guiRotationSpeed;
	ofParameter<float> guiModelScale;
	ofParameter<float> guiLightIntensity;

	// === 方法 ===
	void setupCamera();
	void setupShaders();
	void setupFBO();
	void setupDefaultParams();
	void setupGui();
	void updateFromGui();
	void updateRotation();
	void handleWindowResize(int w, int h);

	void renderToFBO();
	void renderModel();
	void renderUI();

	void setShaderUniforms();
	void setBasicUniforms();
	void setLightingUniforms();
	void setMatrixUniforms();

	void loadDefaultModel();
	void loadModelFromFile(const string & filepath);
	bool validateModel(const ofVboMesh & mesh);

	ofVec3f calculateLightPosition();
	void resetAllParameters();
	void logSystemInfo();
	string getDebugInfo();

	ofMatrix4x4 getModelMatrix() const;
	float getRotationAngle() const { return currentRotationY; }

	// === 位置纹理渲染 ===
	ofFbo positionFBO;
	ofShader positionRenderShader;

	void setupPositionRendering();
	void renderToPositionTexture();

	ofPixels pixels;
};

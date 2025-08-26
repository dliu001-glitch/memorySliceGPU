#pragma once
#include "core/DataManager.h"
#include "geometry/CubeMesh.h"
#include "ofMain.h"
#include "ofxGui.h"
#include "shared/CommonStructs.h"
#include "shared/GeometryData.h"

class Screen2App : public ofBaseApp {
public:
	Screen2App();
	~Screen2App() = default;

	void setup() override;
	void update() override;
	void draw() override;
	void keyPressed(int key) override;

private:
	// === 核心组件 ===
	CubeMesh cubeMesh;
	ofEasyCam cam;
	ofFbo fbo;
	ofShader fractuteShader;
	DataManager & dataManager;

	// === 本地参数缓存 ===
	CubeMeshConfig meshConfig;
	FractureParams fractureParams;
	DissipationParams dissipationParams;
	LightingParams lightingParams;
	FlowFieldConfig flowFieldConfig;

	// === 时间相关 ===
	float elapsedTime = 0.0f;

	// === GUI相关 ===
	ofxPanel gui;
	bool showGui = true;

	// GUI参数组
	ofParameterGroup meshGroup;
	ofParameterGroup fractureGroup;
	ofParameterGroup dissipationGroup;
	ofParameterGroup lightingGroup;

	// 网格和基础动画参数
	ofParameter<float> guiNoiseStrength;
	ofParameter<float> guiBreathAmount;
	ofParameter<float> guiBreathSpeed; // 新增：呼吸速度
	ofParameter<float> guiBreathIntensity; // 新增：呼吸强度倍数
	ofParameter<float> guiBreathContrast; // 新增：呼吸对比度
	ofParameter<float> guiFlowFieldStrength;
	ofParameter<int> guiGridResolution;
	ofParameter<float> guiCubeSize;

	// 破碎效果参数
	ofParameter<bool> guiEnableFracture;
	ofParameter<float> guiFractureAmount;
	ofParameter<float> guiFractureScale;
	ofParameter<float> guiExplosionRadius;
	ofParameter<float> guiRotationIntensity;
	ofParameter<float> guiSeparationForce;

	// 消散效果参数
	ofParameter<bool> guiEnableDissipation;
	ofParameter<float> guiDissipationAmount;
	ofParameter<float> guiDissipationScale;
	ofParameter<float> guiDissipationSpeed;
	ofParameter<float> guiCloudThreshold;
	ofParameter<float> guiEdgeSoftness;

	// 光照参数
	ofParameter<float> guiLightIntensity;
	ofParameter<float> guiAmbientStrength;
	ofParameter<float> guiSpecularShininess;
	ofParameter<bool> guiAutoLightRotation;
	ofParameter<float> guiManualLightAngle;

	

	// === 初始化方法 ===
	void setupCamera();
	void setupShaders();
	void setupFBO();
	void setupDefaultParams();
	void setupMesh();
	void setupGui();

	// === 更新方法 ===
	void updateFromGui();
	void handleWindowResize(int w, int h);

	// === 渲染方法 ===
	void renderToFBO();
	void renderGeometry();
	void renderUI();

	// === Shader参数设置 ===
	void setShaderUniforms();
	void setBasicUniforms();
	void setLightingUniforms();
	void setEffectUniforms();
	void setMatrixUniforms();

	// === 工具方法 ===
	ofVec3f calculateLightPosition();
	void resetAllParameters();
	void logSystemInfo();
	string getDebugInfo();

	// === 位置纹理渲染 ===
	ofFbo positionFBO;
	ofShader positionRenderShader;

	void setupPositionRendering();
	void renderToPositionTexture();
};

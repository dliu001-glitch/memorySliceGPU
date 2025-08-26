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
	// === ������� ===
	CubeMesh cubeMesh;
	ofEasyCam cam;
	ofFbo fbo;
	ofShader fractuteShader;
	DataManager & dataManager;

	// === ���ز������� ===
	CubeMeshConfig meshConfig;
	FractureParams fractureParams;
	DissipationParams dissipationParams;
	LightingParams lightingParams;
	FlowFieldConfig flowFieldConfig;

	// === ʱ����� ===
	float elapsedTime = 0.0f;

	// === GUI��� ===
	ofxPanel gui;
	bool showGui = true;

	// GUI������
	ofParameterGroup meshGroup;
	ofParameterGroup fractureGroup;
	ofParameterGroup dissipationGroup;
	ofParameterGroup lightingGroup;

	// ����ͻ�����������
	ofParameter<float> guiNoiseStrength;
	ofParameter<float> guiBreathAmount;
	ofParameter<float> guiBreathSpeed; // �����������ٶ�
	ofParameter<float> guiBreathIntensity; // ����������ǿ�ȱ���
	ofParameter<float> guiBreathContrast; // �����������Աȶ�
	ofParameter<float> guiFlowFieldStrength;
	ofParameter<int> guiGridResolution;
	ofParameter<float> guiCubeSize;

	// ����Ч������
	ofParameter<bool> guiEnableFracture;
	ofParameter<float> guiFractureAmount;
	ofParameter<float> guiFractureScale;
	ofParameter<float> guiExplosionRadius;
	ofParameter<float> guiRotationIntensity;
	ofParameter<float> guiSeparationForce;

	// ��ɢЧ������
	ofParameter<bool> guiEnableDissipation;
	ofParameter<float> guiDissipationAmount;
	ofParameter<float> guiDissipationScale;
	ofParameter<float> guiDissipationSpeed;
	ofParameter<float> guiCloudThreshold;
	ofParameter<float> guiEdgeSoftness;

	// ���ղ���
	ofParameter<float> guiLightIntensity;
	ofParameter<float> guiAmbientStrength;
	ofParameter<float> guiSpecularShininess;
	ofParameter<bool> guiAutoLightRotation;
	ofParameter<float> guiManualLightAngle;

	

	// === ��ʼ������ ===
	void setupCamera();
	void setupShaders();
	void setupFBO();
	void setupDefaultParams();
	void setupMesh();
	void setupGui();

	// === ���·��� ===
	void updateFromGui();
	void handleWindowResize(int w, int h);

	// === ��Ⱦ���� ===
	void renderToFBO();
	void renderGeometry();
	void renderUI();

	// === Shader�������� ===
	void setShaderUniforms();
	void setBasicUniforms();
	void setLightingUniforms();
	void setEffectUniforms();
	void setMatrixUniforms();

	// === ���߷��� ===
	ofVec3f calculateLightPosition();
	void resetAllParameters();
	void logSystemInfo();
	string getDebugInfo();

	// === λ��������Ⱦ ===
	ofFbo positionFBO;
	ofShader positionRenderShader;

	void setupPositionRendering();
	void renderToPositionTexture();
};

#pragma once
#include "ofMain.h"
#include "shared/CommonStructs.h"
#include "shared/GeometryData.h"
#include <mutex>

class DataManager {
public:
	static DataManager & getInstance();

	// === 动画状态管理 ===
	void setAnimationState(AnimationState state);
	AnimationState getAnimationState() const;

	void setTargetState(AnimationState target);
	AnimationState getTargetState() const;

	void setStateTransition(float transition);
	float getStateTransition() const;

	// === 动画参数管理 ===
	void setAnimationParams(const AnimationParams & params);
	AnimationParams getAnimationParams() const;

	void setCalmParams(const AnimationParams & params);
	AnimationParams getCalmParams() const;

	void setIntenseParams(const AnimationParams & params);
	AnimationParams getIntenseParams() const;

	// === 效果参数管理 ===
	void setFractureParams(const FractureParams & params);
	FractureParams getFractureParams() const;

	void setDissipationParams(const DissipationParams & params);
	DissipationParams getDissipationParams() const;

	// === 光照参数管理 ===
	void setLightingParams(const LightingParams & params);
	LightingParams getLightingParams() const;

	// === 几何数据管理 ===
	void setCubeMeshConfig(const CubeMeshConfig & config);
	CubeMeshConfig getCubeMeshConfig() const;

	void setFlowFieldConfig(const FlowFieldConfig & config);
	FlowFieldConfig getFlowFieldConfig() const;

	// === 渲染数据管理 ===
	void setGeometryFBO(const ofFbo & fbo);
	bool hasGeometryFBO() const;
	const ofFbo & getGeometryFBO() const;

	// === 时间管理 ===
	void setElapsedTime(float time);
	float getElapsedTime() const;

	void setTransitionSpeed(float speed);
	float getTransitionSpeed() const;

	// === 控制设置 ===
	void setDebugMode(bool enabled);
	bool isDebugMode() const;

	void setAutoEffectCycle(bool enabled);
	bool isAutoEffectCycle() const;

	void setEffectStartTime(float time);
	float getEffectStartTime() const;


	void setScreen1Mesh(const ofVboMesh & mesh);
	ofVboMesh getScreen1Mesh() const;
	bool hasScreen1MeshData() const;

	void setScreen2BaseMesh(const ofVboMesh & mesh);
	ofVboMesh getScreen2BaseMesh() const;
	bool hasScreen2MeshData() const;

	ofMatrix4x4 getScreen1ModelMatrix() const;
	void setScreen1ModelMatrix(const ofMatrix4x4 & matrix);
	string getCurrentModelPath() const;
	void setCurrentModelPath(const string & path);

	// === 位置纹理共享 ===
	void setScreen1PositionTexture(const ofTexture & posTexture, const ofTexture & depthTexture);
	void setScreen2PositionTexture(const ofTexture & posTexture, const ofTexture & depthTexture);
	bool hasScreen1PositionData() const;
	bool hasScreen2PositionData() const;
	ofTexture getScreen1PositionTexture() const;
	ofTexture getScreen1DepthTexture() const;
	ofTexture getScreen2PositionTexture() const;
	ofTexture getScreen2DepthTexture() const;


private:
	DataManager() = default;
	~DataManager() = default;
	DataManager(const DataManager &) = delete;
	DataManager & operator=(const DataManager &) = delete;

	mutable std::mutex dataMutex; // 线程安全保护

	// === 数据成员 ===

	// 动画状态
	AnimationState currentState = CALM;
	AnimationState targetState = CALM;
	float stateTransition = 0.0f;
	float transitionSpeed = 0.5f;

	// 动画参数
	AnimationParams currentAnimParams;
	AnimationParams calmParams { 5.0f, 5.0f, 5.0f, 0.0f, 60.0f, 5 };
	AnimationParams intenseParams { 60.0f, 100.0f, 40.0f, 0.8f, 200.0f, 10 };

	// 效果参数
	FractureParams fractureParams;
	DissipationParams dissipationParams;

	// 光照参数
	LightingParams lightingParams;

	// 几何配置
	CubeMeshConfig cubeMeshConfig;
	FlowFieldConfig flowFieldConfig;

	// 渲染数据
	bool hasGeometryFboData = false;
	ofFbo geometryFbo;

	// 时间数据
	float elapsedTime = 0.0f;
	float effectStartTime = 0.0f;

	// 控制设置
	bool debugMode = false;
	bool autoEffectCycle = false;

	// === Mesh数据共享 ===
	ofVboMesh screen1Mesh; // Screen1的模型mesh
	ofVboMesh screen2BaseMesh; // Screen2的基础mesh
	bool hasScreen1Data = false;
	bool hasScreen2Data = false;

	ofMatrix4x4 screen1ModelMatrix = ofMatrix4x4::newIdentityMatrix();
	string currentModelPath = "";

	ofTexture screen1PosTexture, screen1DepthTexture;
	ofTexture screen2PosTexture, screen2DepthTexture;
	bool hasScreen1PosData = false;
	bool hasScreen2PosData = false;
};

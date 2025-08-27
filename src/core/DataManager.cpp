#include "DataManager.h"

DataManager & DataManager::getInstance() {
	static DataManager instance;
	return instance;
}

// === ����״̬���� ===
void DataManager::setAnimationState(AnimationState state) {
	std::lock_guard<std::mutex> lock(dataMutex);
	currentState = state;
}

AnimationState DataManager::getAnimationState() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return currentState;
}

void DataManager::setTargetState(AnimationState target) {
	std::lock_guard<std::mutex> lock(dataMutex);
	targetState = target;
}

AnimationState DataManager::getTargetState() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return targetState;
}

void DataManager::setStateTransition(float transition) {
	std::lock_guard<std::mutex> lock(dataMutex);
	stateTransition = transition;
}

float DataManager::getStateTransition() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return stateTransition;
}

// === ������������ ===

void DataManager::setAnimationParams(const AnimationParams & params) {
	std::lock_guard<std::mutex> lock(dataMutex);
	currentAnimParams = params;
}

AnimationParams DataManager::getAnimationParams() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return currentAnimParams;
}

void DataManager::setCalmParams(const AnimationParams & params) {
	std::lock_guard<std::mutex> lock(dataMutex);
	calmParams = params;
}

AnimationParams DataManager::getCalmParams() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return calmParams;
}

void DataManager::setIntenseParams(const AnimationParams & params) {
	std::lock_guard<std::mutex> lock(dataMutex);
	intenseParams = params;
}

AnimationParams DataManager::getIntenseParams() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return intenseParams;
}

// === Ч���������� ===

void DataManager::setFractureParams(const FractureParams & params) {
	std::lock_guard<std::mutex> lock(dataMutex);
	fractureParams = params;
}

FractureParams DataManager::getFractureParams() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return fractureParams;
}

void DataManager::setDissipationParams(const DissipationParams & params) {
	std::lock_guard<std::mutex> lock(dataMutex);
	dissipationParams = params;
}

DissipationParams DataManager::getDissipationParams() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return dissipationParams;
}

// === ���ղ������� ===

void DataManager::setLightingParams(const LightingParams & params) {
	std::lock_guard<std::mutex> lock(dataMutex);
	lightingParams = params;
}

LightingParams DataManager::getLightingParams() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return lightingParams;
}

// === �������ݹ��� ===

void DataManager::setCubeMeshConfig(const CubeMeshConfig & config) {
	std::lock_guard<std::mutex> lock(dataMutex);
	cubeMeshConfig = config;
}

CubeMeshConfig DataManager::getCubeMeshConfig() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return cubeMeshConfig;
}

void DataManager::setFlowFieldConfig(const FlowFieldConfig & config) {
	std::lock_guard<std::mutex> lock(dataMutex);
	flowFieldConfig = config;
}

FlowFieldConfig DataManager::getFlowFieldConfig() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return flowFieldConfig;
}

// === ��Ⱦ���ݹ��� ===

void DataManager::setGeometryFBO(const ofFbo & fbo) {
	std::lock_guard<std::mutex> lock(dataMutex);
	// ����ֻ�Ǳ����FBO���ݣ�ʵ�ʵ�FBO��Ҫ��ʹ�ô�����
	hasGeometryFboData = true;
}

bool DataManager::hasGeometryFBO() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return hasGeometryFboData;
}

const ofFbo & DataManager::getGeometryFBO() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return geometryFbo;
}

// === ʱ����� ===

void DataManager::setElapsedTime(float time) {
	std::lock_guard<std::mutex> lock(dataMutex);
	elapsedTime = time;
}

float DataManager::getElapsedTime() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return elapsedTime;
}

void DataManager::setTransitionSpeed(float speed) {
	std::lock_guard<std::mutex> lock(dataMutex);
	transitionSpeed = speed;
}

float DataManager::getTransitionSpeed() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return transitionSpeed;
}

// === �������� ===

void DataManager::setDebugMode(bool enabled) {
	std::lock_guard<std::mutex> lock(dataMutex);
	debugMode = enabled;
}

bool DataManager::isDebugMode() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return debugMode;
}

void DataManager::setAutoEffectCycle(bool enabled) {
	std::lock_guard<std::mutex> lock(dataMutex);
	autoEffectCycle = enabled;
}

bool DataManager::isAutoEffectCycle() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return autoEffectCycle;
}

void DataManager::setEffectStartTime(float time) {
	std::lock_guard<std::mutex> lock(dataMutex);
	effectStartTime = time;
}

float DataManager::getEffectStartTime() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return effectStartTime;
}

// === Mesh���ݹ��� ===
void DataManager::setScreen1Mesh(const ofVboMesh & mesh) {
	std::lock_guard<std::mutex> lock(dataMutex);
	screen1Mesh = mesh;
	hasScreen1Data = true;
}

ofVboMesh DataManager::getScreen1Mesh() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return screen1Mesh;
}

bool DataManager::hasScreen1MeshData() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return hasScreen1Data;
}

void DataManager::setScreen2BaseMesh(const ofVboMesh & mesh) {
	std::lock_guard<std::mutex> lock(dataMutex);
	screen2BaseMesh = mesh;
	hasScreen2Data = true;
}

ofVboMesh DataManager::getScreen2BaseMesh() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return screen2BaseMesh;
}

bool DataManager::hasScreen2MeshData() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return hasScreen2Data;
}
void DataManager::setScreen1ModelMatrix(const ofMatrix4x4 & matrix) {
	std::lock_guard<std::mutex> lock(dataMutex);
	screen1ModelMatrix = matrix;
}

ofMatrix4x4 DataManager::getScreen1ModelMatrix() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return screen1ModelMatrix;
}

void DataManager::setCurrentModelPath(const string & path) {
	std::lock_guard<std::mutex> lock(dataMutex);
	currentModelPath = path;
}

string DataManager::getCurrentModelPath() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return currentModelPath;
}
void DataManager::setScreen1PositionTexture(const ofTexture & posTexture, const ofTexture & depthTexture) {
	std::lock_guard<std::mutex> lock(dataMutex);
	ofLogNotice("DataManager") << "Receiving Screen1 texture - ID: " << posTexture.getTextureData().textureID;
	ofLogNotice("DataManager") << "Texture allocated: " << posTexture.isAllocated();
	screen1PosTexture = posTexture;
	screen1DepthTexture = depthTexture;
	hasScreen1PosData = true;
}

void DataManager::setScreen2PositionTexture(const ofTexture & posTexture, const ofTexture & depthTexture) {
	std::lock_guard<std::mutex> lock(dataMutex);
	screen2PosTexture = posTexture;
	screen2DepthTexture = depthTexture;
	hasScreen2PosData = true;
}

bool DataManager::hasScreen1PositionData() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return hasScreen1PosData && screen1PosTexture.isAllocated();
}

bool DataManager::hasScreen2PositionData() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return hasScreen2PosData && screen2PosTexture.isAllocated();
}

ofTexture DataManager::getScreen1PositionTexture() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return screen1PosTexture;
}

ofTexture DataManager::getScreen1DepthTexture() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return screen1DepthTexture;
}

ofTexture DataManager::getScreen2PositionTexture() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return screen2PosTexture;
}

ofTexture DataManager::getScreen2DepthTexture() const {
	std::lock_guard<std::mutex> lock(dataMutex);
	return screen2DepthTexture;
}

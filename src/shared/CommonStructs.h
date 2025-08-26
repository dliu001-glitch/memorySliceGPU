#pragma once
#include "ofMain.h"






// 动画状态枚举
enum AnimationState {
	CALM,
	INTENSE,
	TRANSITION
};
// 光照模式枚举
enum LightingMode {
	WARM_LIGHT,
	COOL_LIGHT,
	COLORFUL_LIGHT,
	DRAMATIC_LIGHT
};

// 动画参数结构
struct AnimationParams{
	float noiseStrength;
	float breathAmount;
	float flowFieldStrength;
	float particleBlend;
	float connectionDistance;
	int emissionRate;

	AnimationParams():
		noiseStrength(20.0f),
		breathAmount(10.0f),
		flowFieldStrength(15.0f),
		particleBlend(0.0f),
		connectionDistance(60.0f),
		emissionRate(5) {}
	AnimationParams(float ns, float ba, float ffs, float pb, float cd, int er):
	noiseStrength(ns), breathAmount(ba), flowFieldStrength(ffs),
	particleBlend(pb), connectionDistance(cd), emissionRate(er) {}
};

// 破碎效果参数
struct FractureParams {
	float fractureAmount = 0.0f; // 破碎强度 0.0-1.0
	float fractureScale = 0.02f; // 破碎噪声缩放
	float explosionRadius = 150.0f; // 爆炸半径
	float rotationIntensity = 0.5f; // 碎片旋转强度
	float separationForce = 50.0f; // 分离力度
	bool enableFracture = false; // 启用破碎效果
};

// 消散效果参数
struct DissipationParams {
	float dissipationAmount = 0.0f; // 消散强度 0.0-1.0
	float dissipationScale = 0.05f; // 消散噪声缩放
	float dissipationSpeed = 1.0f; // 消散动画速度
	float cloudThreshold = 0.4f; // 云状效果阈值
	float edgeSoftness = 0.15f; // 边缘柔和度
	bool enableDissipation = false; // 启用消散效果
};

// 光照参数
struct LightingParams {
	ofVec3f lightColor = ofVec3f(1.0f, 1.0f, 1.0f);
	ofVec3f ambientColor = ofVec3f(0.2f, 0.2f, 0.2f);
	float lightIntensity = 1.0f;
	float ambientStrength = 0.2f;
	float specularShininess = 64.0f;
	bool autoLightRotation = true;
	float manualLightAngle = 0.0f;
	LightingMode lightingMode = WARM_LIGHT;
};

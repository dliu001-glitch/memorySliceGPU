#pragma once
#include "ofMain.h"






// ����״̬ö��
enum AnimationState {
	CALM,
	INTENSE,
	TRANSITION
};
// ����ģʽö��
enum LightingMode {
	WARM_LIGHT,
	COOL_LIGHT,
	COLORFUL_LIGHT,
	DRAMATIC_LIGHT
};

// ���������ṹ
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

// ����Ч������
struct FractureParams {
	float fractureAmount = 0.0f; // ����ǿ�� 0.0-1.0
	float fractureScale = 0.02f; // ������������
	float explosionRadius = 150.0f; // ��ը�뾶
	float rotationIntensity = 0.5f; // ��Ƭ��תǿ��
	float separationForce = 50.0f; // ��������
	bool enableFracture = false; // ��������Ч��
};

// ��ɢЧ������
struct DissipationParams {
	float dissipationAmount = 0.0f; // ��ɢǿ�� 0.0-1.0
	float dissipationScale = 0.05f; // ��ɢ��������
	float dissipationSpeed = 1.0f; // ��ɢ�����ٶ�
	float cloudThreshold = 0.4f; // ��״Ч����ֵ
	float edgeSoftness = 0.15f; // ��Ե��Ͷ�
	bool enableDissipation = false; // ������ɢЧ��
};

// ���ղ���
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

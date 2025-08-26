#pragma once
#include "ofMain.h"

struct CubeMeshConfig {
	int gridResolution = 100;
	float cubeSize = 200.0f;

	float noiseScale = 0.05f;              // Perlin��������
	float noiseStrength = 20.0f;           // ����ǿ��
	float breathSpeed = 0.5f;              // �����ٶ�
	float breathAmount = 10.0f;            // ��������
	float flowFieldStrength = 15.0f;       // ����ǿ��
};


struct VertexPoolData { 
	map<string, int> vertexMap;             // ����λ�õ�������ӳ��
	vector<ofVec3f> vertexPool;             // �������
	vector<ofVec3f> originalVertices;       // ԭʼ����λ��
	vector<ofVec3f> vertexNormals;          // ���㷨����

	int vertexIndexCounter = 0;             // ��������������

	void clear() {
		vertexMap.clear();
		vertexPool.clear();
		originalVertices.clear();
		vertexNormals.clear();
		vertexIndexCounter = 0;
	}
};
// �������ĵ�����
struct FlowFieldConfig {
	ofVec3f flowCenter1 = ofVec3f(80.0f, 80.0f, 80.0f); // ��50���ӵ�80
	ofVec3f flowCenter2 = ofVec3f(-80.0f, -80.0f, 80.0f);
	ofVec3f flowCenter3 = ofVec3f(80.0f, 80.0f, -80.0f);

	// ����5�����ĵ㣬�γɸ��õĸ���
	ofVec3f flowCenter4 = ofVec3f(-80.0f, 80.0f, -80.0f);
	ofVec3f flowCenter5 = ofVec3f(0.0f, 0.0f, 100.0f);
	ofVec3f flowCenter6 = ofVec3f(100.0f, 0.0f, 0.0f);
	ofVec3f flowCenter7 = ofVec3f(-100.0f, 0.0f, 0.0f);
	ofVec3f flowCenter8 = ofVec3f(0.0f, 100.0f, 0.0f);

	int centerCount = 8; // ���ĵ�����
};

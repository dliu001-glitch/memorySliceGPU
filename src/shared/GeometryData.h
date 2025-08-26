#pragma once
#include "ofMain.h"

struct CubeMeshConfig {
	int gridResolution = 100;
	float cubeSize = 200.0f;

	float noiseScale = 0.05f;              // Perlin噪声缩放
	float noiseStrength = 20.0f;           // 噪声强度
	float breathSpeed = 0.5f;              // 呼吸速度
	float breathAmount = 10.0f;            // 呼吸幅度
	float flowFieldStrength = 15.0f;       // 流场强度
};


struct VertexPoolData { 
	map<string, int> vertexMap;             // 顶点位置到索引的映射
	vector<ofVec3f> vertexPool;             // 共享顶点池
	vector<ofVec3f> originalVertices;       // 原始顶点位置
	vector<ofVec3f> vertexNormals;          // 顶点法向量

	int vertexIndexCounter = 0;             // 顶点索引计数器

	void clear() {
		vertexMap.clear();
		vertexPool.clear();
		originalVertices.clear();
		vertexNormals.clear();
		vertexIndexCounter = 0;
	}
};
// 流场中心点配置
struct FlowFieldConfig {
	ofVec3f flowCenter1 = ofVec3f(80.0f, 80.0f, 80.0f); // 从50增加到80
	ofVec3f flowCenter2 = ofVec3f(-80.0f, -80.0f, 80.0f);
	ofVec3f flowCenter3 = ofVec3f(80.0f, 80.0f, -80.0f);

	// 新增5个中心点，形成更好的覆盖
	ofVec3f flowCenter4 = ofVec3f(-80.0f, 80.0f, -80.0f);
	ofVec3f flowCenter5 = ofVec3f(0.0f, 0.0f, 100.0f);
	ofVec3f flowCenter6 = ofVec3f(100.0f, 0.0f, 0.0f);
	ofVec3f flowCenter7 = ofVec3f(-100.0f, 0.0f, 0.0f);
	ofVec3f flowCenter8 = ofVec3f(0.0f, 100.0f, 0.0f);

	int centerCount = 8; // 中心点数量
};

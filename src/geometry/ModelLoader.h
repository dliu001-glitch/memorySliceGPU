#pragma once
#include "ofMain.h"

class ModelLoader {
public:
	ModelLoader();
	~ModelLoader();

	// 主要加载方法
	bool loadModel(const string & filepath, ofVboMesh & outMesh);

	// 支持的格式检查
	bool isSupportedFormat(const string & filepath);
	vector<string> getSupportedFormats();

	// 模型信息
	struct ModelInfo {
		int vertexCount;
		int indexCount;
		ofVec3f boundingBoxMin;
		ofVec3f boundingBoxMax;
		ofVec3f center;
		float maxDimension;
		bool hasNormals;
		bool hasTexCoords;
		bool hasColors;
	};

	ModelInfo getLastLoadedInfo() const { return lastModelInfo; }

	// 配置选项
	struct LoadOptions {
		bool generateNormals = true;
		bool flipNormals = false;
		bool centerModel = true;
		bool normalizeSize = true;
		float targetSize = 100.0f;
		bool smoothNormals = true;
	};

	void setLoadOptions(const LoadOptions & options) { loadOptions = options; }
	LoadOptions getLoadOptions() const { return loadOptions; }

private:
	bool loadOBJ(const string & filepath, ofVboMesh & outMesh);
	bool loadPLY(const string & filepath, ofVboMesh & outMesh);

	// 后处理方法
	void postProcessMesh(ofVboMesh & mesh);
	void generateNormals(ofVboMesh & mesh);
	void centerAndNormalizeMesh(ofVboMesh & mesh);
	void calculateBoundingBox(const ofVboMesh & mesh, ofVec3f & min, ofVec3f & max);
	void smoothNormals(ofVboMesh & mesh);

	// 验证和修复
	bool validateMesh(const ofVboMesh & mesh);
	void removeDuplicateVertices(ofVboMesh & mesh);

	// 模型信息计算
	void calculateModelInfo(const ofVboMesh & mesh);

	// 内部状态
	LoadOptions loadOptions;
	ModelInfo lastModelInfo;

	// 支持的格式列表
	vector<string> supportedFormats;

	// 初始化
	void initializeSupportedFormats();
};

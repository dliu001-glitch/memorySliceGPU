#pragma once
#include "ofMain.h"

class ModelLoader {
public:
	ModelLoader();
	~ModelLoader();

	// ��Ҫ���ط���
	bool loadModel(const string & filepath, ofVboMesh & outMesh);

	// ֧�ֵĸ�ʽ���
	bool isSupportedFormat(const string & filepath);
	vector<string> getSupportedFormats();

	// ģ����Ϣ
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

	// ����ѡ��
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

	// ������
	void postProcessMesh(ofVboMesh & mesh);
	void generateNormals(ofVboMesh & mesh);
	void centerAndNormalizeMesh(ofVboMesh & mesh);
	void calculateBoundingBox(const ofVboMesh & mesh, ofVec3f & min, ofVec3f & max);
	void smoothNormals(ofVboMesh & mesh);

	// ��֤���޸�
	bool validateMesh(const ofVboMesh & mesh);
	void removeDuplicateVertices(ofVboMesh & mesh);

	// ģ����Ϣ����
	void calculateModelInfo(const ofVboMesh & mesh);

	// �ڲ�״̬
	LoadOptions loadOptions;
	ModelInfo lastModelInfo;

	// ֧�ֵĸ�ʽ�б�
	vector<string> supportedFormats;

	// ��ʼ��
	void initializeSupportedFormats();
};

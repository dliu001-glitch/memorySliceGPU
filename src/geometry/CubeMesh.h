#pragma once
#include "ofMain.h"
#include "shared/GeometryData.h"


class CubeMesh {
public:
	CubeMesh();
	~CubeMesh();

	//Initialize
	void setup(const CubeMeshConfig& config);
	void clear();
	//Mesh Generate
	void generateMesh();
	void regenerateWithResolution(int resolution);

	//visitor
	const ofVboMesh & getMesh() const { return mesh; }
	ofVboMesh & getMesh() { return mesh; }

	const std::vector<ofVec3f> & getOriginalVertices() const { return vertexPoolData.originalVertices; }
	const std::vector<ofVec3f> & getVertexPool() const { return vertexPoolData.vertexPool; }
	const std::vector<ofVec3f> & getVertexNormals() const { return vertexPoolData.vertexNormals; }

	
    // ���÷���
	const CubeMeshConfig & getConfig() const { return config; }
	void updateConfig(const CubeMeshConfig & newConfig);

	// ͳ����Ϣ
	int getVertexCount() const;
	int getIndexCount() const;
	float getVertexSharingRatio() const; // ���㹲��Ч��

	// ����
	void logMeshInfo() const;

private:
	// ��������
	CubeMeshConfig config;

	// ��������
	ofVboMesh mesh;
	VertexPoolData vertexPoolData;

	// �ڲ���������
	void createCubeMesh();
	void addFaceWithNormal(const ofVec3f & origin, const ofVec3f & right, const ofVec3f & down, const ofVec3f & faceNormal);
	int getVertexIndexWithNormal(const ofVec3f & pos, const ofVec3f & normal);

	// ����������ɫ
	ofColor generateVertexColor(const ofVec3f & position) const;

	// ���߷���
	std::string vectorToKey(const ofVec3f & vec) const;

};

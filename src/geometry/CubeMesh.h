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

	
    // 配置访问
	const CubeMeshConfig & getConfig() const { return config; }
	void updateConfig(const CubeMeshConfig & newConfig);

	// 统计信息
	int getVertexCount() const;
	int getIndexCount() const;
	float getVertexSharingRatio() const; // 顶点共享效率

	// 调试
	void logMeshInfo() const;

private:
	// 配置数据
	CubeMeshConfig config;

	// 网格数据
	ofVboMesh mesh;
	VertexPoolData vertexPoolData;

	// 内部辅助方法
	void createCubeMesh();
	void addFaceWithNormal(const ofVec3f & origin, const ofVec3f & right, const ofVec3f & down, const ofVec3f & faceNormal);
	int getVertexIndexWithNormal(const ofVec3f & pos, const ofVec3f & normal);

	// 生成网格颜色
	ofColor generateVertexColor(const ofVec3f & position) const;

	// 工具方法
	std::string vectorToKey(const ofVec3f & vec) const;

};

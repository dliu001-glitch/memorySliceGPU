
#include "CubeMesh.h"
#include <algorithm>

CubeMesh::CubeMesh() {
}

CubeMesh::~CubeMesh() {
	clear();
}

void CubeMesh::setup(const CubeMeshConfig & meshConfig) {
	config = meshConfig;
	generateMesh();
}

void CubeMesh::clear() {
	mesh.clear();
	vertexPoolData.clear();
}

void CubeMesh::generateMesh() {
	clear();
	createCubeMesh();
	logMeshInfo();
}

void CubeMesh::regenerateWithResolution(int resolution) {
	config.gridResolution = resolution;
	generateMesh();
}

void CubeMesh::createCubeMesh() {
	float offset = config.cubeSize / 2.0f;
	mesh.clear();
	mesh.setMode(OF_PRIMITIVE_TRIANGLES); // 使用三角形模式以支持光照

	vertexPoolData.clear();
	float half = config.cubeSize / 2.0f;

	// 定义6个面及其法向量
	struct FaceData {
		ofVec3f origin;
		ofVec3f right;
		ofVec3f down;
		ofVec3f normal;
		std::string name;
	};

	std::vector<FaceData> faces = {
		// 前面 (Z+)
		{ ofVec3f(-half, -half, +half), ofVec3f(1, 0, 0), ofVec3f(0, 1, 0), ofVec3f(0, 0, 1), "Front" },
		// 后面 (Z-)
		{ ofVec3f(+half, -half, -half), ofVec3f(-1, 0, 0), ofVec3f(0, 1, 0), ofVec3f(0, 0, -1), "Back" },
		// 上面 (Y+)
		{ ofVec3f(-half, +half, +half), ofVec3f(1, 0, 0), ofVec3f(0, 0, -1), ofVec3f(0, 1, 0), "Top" },
		// 下面 (Y-)
		{ ofVec3f(-half, -half, -half), ofVec3f(1, 0, 0), ofVec3f(0, 0, 1), ofVec3f(0, -1, 0), "Bottom" },
		// 左面 (X-)
		{ ofVec3f(-half, -half, -half), ofVec3f(0, 1, 0), ofVec3f(0, 0, 1), ofVec3f(-1, 0, 0), "Left" },
		// 右面 (X+)
		{ ofVec3f(+half, -half, +half), ofVec3f(0, 1, 0), ofVec3f(0, 0, -1), ofVec3f(1, 0, 0), "Right" }
	};

	// 为每个面生成顶点和索引
	for (const auto & face : faces) {
		addFaceWithNormal(face.origin, face.right, face.down, face.normal);
	}

	// 确保vertexNormals和vertexPool大小一致
	if (vertexPoolData.vertexNormals.size() < vertexPoolData.vertexPool.size()) {
		vertexPoolData.vertexNormals.resize(vertexPoolData.vertexPool.size());
	}

	// 添加共享顶点到mesh
	for (size_t i = 0; i < vertexPoolData.vertexPool.size(); i++) {
		mesh.addVertex(vertexPoolData.vertexPool[i]);
		mesh.addNormal(vertexPoolData.vertexNormals[i]);
		mesh.addColor(generateVertexColor(vertexPoolData.vertexPool[i]));

		// 生成纹理坐标
		ofVec3f normPos = (vertexPoolData.vertexPool[i] + ofVec3f(offset)) / config.cubeSize;
		mesh.addTexCoord(ofVec2f(normPos.x + normPos.y, normPos.z));
	}
}

void CubeMesh::addFaceWithNormal(const ofVec3f & origin, const ofVec3f & right, const ofVec3f & down, const ofVec3f & faceNormal) {
	float step = config.cubeSize / config.gridResolution;

	for (int j = 0; j < config.gridResolution; j++) {
		for (int i = 0; i < config.gridResolution; i++) {
			ofVec3f v00 = origin + right * i * step + down * j * step;
			ofVec3f v10 = origin + right * (i + 1) * step + down * j * step;
			ofVec3f v01 = origin + right * i * step + down * (j + 1) * step;
			ofVec3f v11 = origin + right * (i + 1) * step + down * (j + 1) * step;

			int i0 = getVertexIndexWithNormal(v00, faceNormal);
			int i1 = getVertexIndexWithNormal(v10, faceNormal);
			int i2 = getVertexIndexWithNormal(v01, faceNormal);
			int i3 = getVertexIndexWithNormal(v11, faceNormal);

			// 两个三角形构成一个quad
			// 第一个三角形: v00 -> v10 -> v01
			mesh.addIndex(i0);
			mesh.addIndex(i1);
			mesh.addIndex(i2);

			// 第二个三角形: v10 -> v11 -> v01
			mesh.addIndex(i1);
			mesh.addIndex(i3);
			mesh.addIndex(i2);
		}
	}
}

int CubeMesh::getVertexIndexWithNormal(const ofVec3f & pos, const ofVec3f & normal) {
	std::string key = vectorToKey(pos);

	if (vertexPoolData.vertexMap.count(key)) {
		// 顶点已存在，平均法向量（用于共享顶点的平滑着色）
		int existingIndex = vertexPoolData.vertexMap[key];
		if (existingIndex < (int)vertexPoolData.vertexNormals.size()) {
			vertexPoolData.vertexNormals[existingIndex] = (vertexPoolData.vertexNormals[existingIndex] + normal).getNormalized();
		}
		return existingIndex;
	} else {
		// 新顶点
		vertexPoolData.vertexMap[key] = vertexPoolData.vertexIndexCounter;
		vertexPoolData.vertexPool.push_back(pos);
		vertexPoolData.originalVertices.push_back(pos);
		vertexPoolData.vertexNormals.push_back(normal);
		return vertexPoolData.vertexIndexCounter++;
	}
}

ofColor CubeMesh::generateVertexColor(const ofVec3f & position) const {
	// 生成白色用于线框显示
	return ofColor(255, 255, 255, 200);
}

std::string CubeMesh::vectorToKey(const ofVec3f & vec) const {
	// 使用一定精度避免浮点数误差
	float precision = 1000.0f;
	int x = (int)(vec.x * precision);
	int y = (int)(vec.y * precision);
	int z = (int)(vec.z * precision);
	return std::to_string(x) + "_" + std::to_string(y) + "_" + std::to_string(z);
}

void CubeMesh::updateConfig(const CubeMeshConfig & newConfig) {
	bool needRegenerate = (config.gridResolution != newConfig.gridResolution || config.cubeSize != newConfig.cubeSize);

	config = newConfig;

	if (needRegenerate) {
		generateMesh();
	}
}

int CubeMesh::getVertexCount() const {
	return mesh.getNumVertices();
}

int CubeMesh::getIndexCount() const {
	return mesh.getNumIndices();
}

float CubeMesh::getVertexSharingRatio() const {
	if (mesh.getNumVertices() == 0) return 0.0f;
	return (float)mesh.getNumIndices() / mesh.getNumVertices();
}

void CubeMesh::logMeshInfo() const {
	ofLogNotice("CubeMesh") << "Created optimized cube mesh:";
	ofLogNotice("CubeMesh") << "  Grid Resolution: " << config.gridResolution << "x" << config.gridResolution << "x" << config.gridResolution;
	ofLogNotice("CubeMesh") << "  Cube Size: " << config.cubeSize;
	ofLogNotice("CubeMesh") << "  Vertices: " << getVertexCount() << " (shared via vertexPool)";
	ofLogNotice("CubeMesh") << "  Indices: " << getIndexCount();
	ofLogNotice("CubeMesh") << "  Vertex sharing efficiency: " << getVertexSharingRatio() << ":1";
}

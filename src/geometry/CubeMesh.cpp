
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
	mesh.setMode(OF_PRIMITIVE_TRIANGLES); // ʹ��������ģʽ��֧�ֹ���

	vertexPoolData.clear();
	float half = config.cubeSize / 2.0f;

	// ����6���漰�䷨����
	struct FaceData {
		ofVec3f origin;
		ofVec3f right;
		ofVec3f down;
		ofVec3f normal;
		std::string name;
	};

	std::vector<FaceData> faces = {
		// ǰ�� (Z+)
		{ ofVec3f(-half, -half, +half), ofVec3f(1, 0, 0), ofVec3f(0, 1, 0), ofVec3f(0, 0, 1), "Front" },
		// ���� (Z-)
		{ ofVec3f(+half, -half, -half), ofVec3f(-1, 0, 0), ofVec3f(0, 1, 0), ofVec3f(0, 0, -1), "Back" },
		// ���� (Y+)
		{ ofVec3f(-half, +half, +half), ofVec3f(1, 0, 0), ofVec3f(0, 0, -1), ofVec3f(0, 1, 0), "Top" },
		// ���� (Y-)
		{ ofVec3f(-half, -half, -half), ofVec3f(1, 0, 0), ofVec3f(0, 0, 1), ofVec3f(0, -1, 0), "Bottom" },
		// ���� (X-)
		{ ofVec3f(-half, -half, -half), ofVec3f(0, 1, 0), ofVec3f(0, 0, 1), ofVec3f(-1, 0, 0), "Left" },
		// ���� (X+)
		{ ofVec3f(+half, -half, +half), ofVec3f(0, 1, 0), ofVec3f(0, 0, -1), ofVec3f(1, 0, 0), "Right" }
	};

	// Ϊÿ�������ɶ��������
	for (const auto & face : faces) {
		addFaceWithNormal(face.origin, face.right, face.down, face.normal);
	}

	// ȷ��vertexNormals��vertexPool��Сһ��
	if (vertexPoolData.vertexNormals.size() < vertexPoolData.vertexPool.size()) {
		vertexPoolData.vertexNormals.resize(vertexPoolData.vertexPool.size());
	}

	// ��ӹ����㵽mesh
	for (size_t i = 0; i < vertexPoolData.vertexPool.size(); i++) {
		mesh.addVertex(vertexPoolData.vertexPool[i]);
		mesh.addNormal(vertexPoolData.vertexNormals[i]);
		mesh.addColor(generateVertexColor(vertexPoolData.vertexPool[i]));

		// ������������
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

			// ���������ι���һ��quad
			// ��һ��������: v00 -> v10 -> v01
			mesh.addIndex(i0);
			mesh.addIndex(i1);
			mesh.addIndex(i2);

			// �ڶ���������: v10 -> v11 -> v01
			mesh.addIndex(i1);
			mesh.addIndex(i3);
			mesh.addIndex(i2);
		}
	}
}

int CubeMesh::getVertexIndexWithNormal(const ofVec3f & pos, const ofVec3f & normal) {
	std::string key = vectorToKey(pos);

	if (vertexPoolData.vertexMap.count(key)) {
		// �����Ѵ��ڣ�ƽ�������������ڹ������ƽ����ɫ��
		int existingIndex = vertexPoolData.vertexMap[key];
		if (existingIndex < (int)vertexPoolData.vertexNormals.size()) {
			vertexPoolData.vertexNormals[existingIndex] = (vertexPoolData.vertexNormals[existingIndex] + normal).getNormalized();
		}
		return existingIndex;
	} else {
		// �¶���
		vertexPoolData.vertexMap[key] = vertexPoolData.vertexIndexCounter;
		vertexPoolData.vertexPool.push_back(pos);
		vertexPoolData.originalVertices.push_back(pos);
		vertexPoolData.vertexNormals.push_back(normal);
		return vertexPoolData.vertexIndexCounter++;
	}
}

ofColor CubeMesh::generateVertexColor(const ofVec3f & position) const {
	// ���ɰ�ɫ�����߿���ʾ
	return ofColor(255, 255, 255, 200);
}

std::string CubeMesh::vectorToKey(const ofVec3f & vec) const {
	// ʹ��һ�����ȱ��⸡�������
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

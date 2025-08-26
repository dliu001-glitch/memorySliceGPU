#include "ModelLoader.h"
#include <algorithm>

ModelLoader::ModelLoader() {
	initializeSupportedFormats();

	// ����Ĭ�ϼ���ѡ��
	loadOptions.generateNormals = true;
	loadOptions.centerModel = true;
	loadOptions.normalizeSize = true;
	loadOptions.targetSize = 100.0f;
	loadOptions.smoothNormals = true;
}

ModelLoader::~ModelLoader() {
}

//--------------------------------------------------------------
void ModelLoader::initializeSupportedFormats() {
	supportedFormats.clear();
	supportedFormats.push_back("obj");
	supportedFormats.push_back("ply");
	// ���Ը�����Ҫ��Ӹ����ʽ
}

//--------------------------------------------------------------
bool ModelLoader::isSupportedFormat(const string & filepath) {
	string extension = ofToLower(ofFilePath::getFileExt(filepath));
	return std::find(supportedFormats.begin(), supportedFormats.end(), extension) != supportedFormats.end();
}

//--------------------------------------------------------------
vector<string> ModelLoader::getSupportedFormats() {
	return supportedFormats;
}

//--------------------------------------------------------------
bool ModelLoader::loadModel(const string & filepath, ofVboMesh & outMesh) {
	if (!ofFile::doesFileExist(filepath)) {
		ofLogError("ModelLoader") << "File does not exist: " << filepath;
		return false;
	}

	if (!isSupportedFormat(filepath)) {
		ofLogError("ModelLoader") << "Unsupported format: " << filepath;
		return false;
	}

	string extension = ofToLower(ofFilePath::getFileExt(filepath));
	bool success = false;

	// ������mesh
	outMesh.clear();

	// ������չ��������Ӧ�ļ�����
	if (extension == "obj") {
		success = loadOBJ(filepath, outMesh);
	} else if (extension == "ply") {
		success = loadPLY(filepath, outMesh);
	}

	if (success && validateMesh(outMesh)) {
		// ����
		postProcessMesh(outMesh);
		ofLogNotice("ModelLoader") << "Successfully loaded: " << filepath;
		return true;
	} else {
		outMesh.clear();
		ofLogError("ModelLoader") << "Failed to load or validate: " << filepath;
		return false;
	}
}

//--------------------------------------------------------------
bool ModelLoader::loadOBJ(const string & filepath, ofVboMesh & outMesh) {
	ofLogNotice("ModelLoader") << "Loading OBJ file: " << filepath;

	ofFile file(filepath);
	if (!file.exists()) {
		return false;
	}

	vector<ofVec3f> vertices;
	vector<ofVec3f> normals;
	vector<ofVec2f> texCoords;
	vector<ofIndexType> indices;

	string line;
	ofBuffer buffer = file.readToBuffer();

	for (auto & line : buffer.getLines()) {
		if (line.empty() || line[0] == '#') continue;

		vector<string> tokens = ofSplitString(line, " ");
		if (tokens.empty()) continue;

		if (tokens[0] == "v" && tokens.size() >= 4) {
			// ����λ��
			float x = ofToFloat(tokens[1]);
			float y = ofToFloat(tokens[2]);
			float z = ofToFloat(tokens[3]);
			vertices.push_back(ofVec3f(x, y, z));

		} else if (tokens[0] == "vn" && tokens.size() >= 4) {
			// ���㷨��
			float x = ofToFloat(tokens[1]);
			float y = ofToFloat(tokens[2]);
			float z = ofToFloat(tokens[3]);
			normals.push_back(ofVec3f(x, y, z));

		} else if (tokens[0] == "vt" && tokens.size() >= 3) {
			// ��������
			float u = ofToFloat(tokens[1]);
			float v = ofToFloat(tokens[2]);
			texCoords.push_back(ofVec2f(u, v));

		} else if (tokens[0] == "f" && tokens.size() >= 4) {
			// �涨�� (֧�������κ��ı���)
			vector<int> faceVertices;
			vector<int> faceNormals;
			vector<int> faceTexCoords;

			for (int i = 1; i < tokens.size(); i++) {
				vector<string> faceTokens = ofSplitString(tokens[i], "/");
				if (!faceTokens.empty()) {
					int vertexIndex = ofToInt(faceTokens[0]) - 1; // OBJ������1��ʼ
					faceVertices.push_back(vertexIndex);

					if (faceTokens.size() > 1 && !faceTokens[1].empty()) {
						int texIndex = ofToInt(faceTokens[1]) - 1;
						faceTexCoords.push_back(texIndex);
					}

					if (faceTokens.size() > 2 && !faceTokens[2].empty()) {
						int normalIndex = ofToInt(faceTokens[2]) - 1;
						faceNormals.push_back(normalIndex);
					}
				}
			}

			// ����������ǻ�
			for (int i = 1; i < faceVertices.size() - 1; i++) {
				indices.push_back(faceVertices[0]);
				indices.push_back(faceVertices[i]);
				indices.push_back(faceVertices[i + 1]);
			}
		}
	}

	if (vertices.empty()) {
		ofLogError("ModelLoader") << "No vertices found in OBJ file";
		return false;
	}

	// ����mesh
	outMesh.clear();
	for (const auto & vertex : vertices) {
		outMesh.addVertex(vertex);
	}

	// ��ӷ��ߣ������������ƥ�䣩
	if (!normals.empty() && normals.size() == vertices.size()) {
		for (const auto & normal : normals) {
			outMesh.addNormal(normal);
		}
	}

	// ����������꣨�����������ƥ�䣩
	if (!texCoords.empty() && texCoords.size() == vertices.size()) {
		for (const auto & texCoord : texCoords) {
			outMesh.addTexCoord(texCoord);
		}
	}

	// �������
	for (const auto & index : indices) {
		outMesh.addIndex(index);
	}

	ofLogNotice("ModelLoader") << "OBJ loaded - Vertices: " << vertices.size()
							   << ", Indices: " << indices.size();

	return true;
}

//--------------------------------------------------------------
bool ModelLoader::loadPLY(const string & filepath, ofVboMesh & outMesh) {
	// PLY�������ļ�ʵ��
	// ������Ը�����Ҫʵ��PLY��ʽ֧��
	ofLogWarning("ModelLoader") << "PLY format not yet implemented";
	return false;
}

//--------------------------------------------------------------
void ModelLoader::postProcessMesh(ofVboMesh & mesh) {
	// 1. ���ɷ��� (�����Ҫ��û��)
	if (loadOptions.generateNormals && !mesh.hasNormals()) {
		generateNormals(mesh);
	}

	// 2. ��ת���� (�����Ҫ)
	if (loadOptions.flipNormals && mesh.hasNormals()) {
		auto & normals = mesh.getNormals();
		for (auto & normal : normals) {
			normal *= -1;
		}
	}

	// 3. ���кͱ�׼��
	if (loadOptions.centerModel || loadOptions.normalizeSize) {
		centerAndNormalizeMesh(mesh);
	}

	// 4. ƽ������
	if (loadOptions.smoothNormals && mesh.hasNormals()) {
		smoothNormals(mesh);
	}

	// 5. ����ģ����Ϣ
	calculateModelInfo(mesh);
}

//--------------------------------------------------------------
void ModelLoader::generateNormals(ofVboMesh & mesh) {
	if (mesh.getNumVertices() == 0) return;

	vector<ofVec3f> normals(mesh.getNumVertices(), ofVec3f(0, 0, 0));
	auto & vertices = mesh.getVertices();
	auto & indices = mesh.getIndices();

	// �����淨�߲��ۼӵ����㷨��
	for (int i = 0; i < indices.size(); i += 3) {
		if (i + 2 < indices.size()) {
			int i0 = indices[i];
			int i1 = indices[i + 1];
			int i2 = indices[i + 2];

			if (i0 < vertices.size() && i1 < vertices.size() && i2 < vertices.size()) {
				ofVec3f v0 = vertices[i0];
				ofVec3f v1 = vertices[i1];
				ofVec3f v2 = vertices[i2];

				ofVec3f edge1 = v1 - v0;
				ofVec3f edge2 = v2 - v0;
				ofVec3f faceNormal = edge1.getCrossed(edge2);

				normals[i0] += faceNormal;
				normals[i1] += faceNormal;
				normals[i2] += faceNormal;
			}
		}
	}

	// ��һ������
	for (auto & normal : normals) {
		normal.normalize();
	}

	mesh.clearNormals();
	for (const auto & normal : normals) {
		mesh.addNormal(normal);
	}
	//mesh.addNormals(normals);
}

//--------------------------------------------------------------
void ModelLoader::centerAndNormalizeMesh(ofVboMesh & mesh) {
	if (mesh.getNumVertices() == 0) return;

	ofVec3f minBounds, maxBounds;
	calculateBoundingBox(mesh, minBounds, maxBounds);

	ofVec3f center = (minBounds + maxBounds) * 0.5f;
	ofVec3f size = maxBounds - minBounds;
	float maxDimension = std::max({ size.x, size.y, size.z });

	auto & vertices = mesh.getVertices();

	// ����
	if (loadOptions.centerModel) {
		for (auto & vertex : vertices) {
			vertex -= center;
		}
	}

	// ��׼���ߴ�
	if (loadOptions.normalizeSize && maxDimension > 0) {
		float scale = loadOptions.targetSize / maxDimension;
		for (auto & vertex : vertices) {
			vertex *= scale;
		}
	}
}

//--------------------------------------------------------------
void ModelLoader::calculateBoundingBox(const ofVboMesh & mesh, ofVec3f & min, ofVec3f & max) {
	if (mesh.getNumVertices() == 0) {
		min = max = ofVec3f(0, 0, 0);
		return;
	}

	auto & vertices = mesh.getVertices();
	min = max = vertices[0];

	for (const auto & vertex : vertices) {
		min.x = std::min(min.x, vertex.x);
		min.y = std::min(min.y, vertex.y);
		min.z = std::min(min.z, vertex.z);

		max.x = std::max(max.x, vertex.x);
		max.y = std::max(max.y, vertex.y);
		max.z = std::max(max.z, vertex.z);
	}
}

//--------------------------------------------------------------
void ModelLoader::smoothNormals(ofVboMesh & mesh) {
	// �򵥵ķ���ƽ���㷨
	// ���Ը�����Ҫʵ�ָ����ӵ�ƽ���㷨
}

//--------------------------------------------------------------
bool ModelLoader::validateMesh(const ofVboMesh & mesh) {
	if (mesh.getNumVertices() == 0) {
		ofLogError("ModelLoader") << "Mesh has no vertices";
		return false;
	}

	if (mesh.getNumIndices() > 0 && mesh.getNumIndices() % 3 != 0) {
		ofLogWarning("ModelLoader") << "Index count is not multiple of 3";
	}

	return true;
}

//--------------------------------------------------------------
void ModelLoader::calculateModelInfo(const ofVboMesh & mesh) {
	lastModelInfo.vertexCount = mesh.getNumVertices();
	lastModelInfo.indexCount = mesh.getNumIndices();
	lastModelInfo.hasNormals = mesh.hasNormals();
	lastModelInfo.hasTexCoords = mesh.hasTexCoords();
	lastModelInfo.hasColors = mesh.hasColors();

	calculateBoundingBox(mesh, lastModelInfo.boundingBoxMin, lastModelInfo.boundingBoxMax);
	lastModelInfo.center = (lastModelInfo.boundingBoxMin + lastModelInfo.boundingBoxMax) * 0.5f;

	ofVec3f size = lastModelInfo.boundingBoxMax - lastModelInfo.boundingBoxMin;
	lastModelInfo.maxDimension = std::max({ size.x, size.y, size.z });
}
//--------------------------------------------------------------

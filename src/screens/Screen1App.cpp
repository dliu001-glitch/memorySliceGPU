#include "Screen1App.h"

Screen1App::Screen1App()
	: dataManager(DataManager::getInstance()) {
}

//--------------------------------------------------------------
void Screen1App::setup() {
	ofLogNotice("Screen1App") << "Initializing model display screen...";

	setupCamera();
	setupShaders();
	setupFBO();
	setupDefaultParams();
	setupGui();
	setupPositionRendering();
	// ����Ĭ��ģ��
	loadDefaultModel();

	logSystemInfo();
}

//--------------------------------------------------------------
void Screen1App::setupCamera() {
	cam.setDistance(400);
	cam.setNearClip(1.0f);
	cam.setFarClip(2000.0f);
}

//--------------------------------------------------------------
void Screen1App::setupShaders() {
	// ���shader�ļ��Ƿ����
	string vertPath = "shaders/model/basic.vert";
	string fragPath = "shaders/model/basic.frag";

	if (ofFile::doesFileExist(vertPath) && ofFile::doesFileExist(fragPath)) {
		if (!modelShader.load("shaders/model/basic")) {
			ofLogWarning("Screen1App") << "Model shader failed to load, using basic rendering";
		} else {
			ofLogNotice("Screen1App") << "Model shader loaded successfully";
		}
	} else {
		ofLogNotice("Screen1App") << "Model shader files not found, using basic rendering";
		ofLogNotice("Screen1App") << "Expected: " << vertPath << " and " << fragPath;
	}
}

//--------------------------------------------------------------
void Screen1App::setupFBO() {
	fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
}

//--------------------------------------------------------------
void Screen1App::setupDefaultParams() {
	// ��ʼ�����ղ���
	lightingParams = LightingParams();

	// ����Ĭ��ģ�Ͳ���
	modelPosition = ofVec3f(0, 0, 0);
	modelScale = ofVec3f(1, 1, 1);
	rotationSpeed = 30.0f;
	currentRotationY = 0.0f;
}

//--------------------------------------------------------------
void Screen1App::setupGui() {
	gui.setup("Model Control Panel");
	gui.setDefaultWidth(250);

	// ʹ��Ĭ��ֵ��ʼ��GUI����
	guiAutoRotation.set("Auto Rotation", true);
	guiRotationSpeed.set("Rotation Speed", rotationSpeed, 0.0f, 180.0f);
	guiModelScale.set("Model Scale", 1.0f, 0.1f, 5.0f);
	guiLightIntensity.set("Light Intensity", lightingParams.lightIntensity, 0.0f, 3.0f);

	// ��ӵ�GUI
	gui.add(guiAutoRotation);
	gui.add(guiRotationSpeed);
	gui.add(guiModelScale);
	gui.add(guiLightIntensity);
}

//--------------------------------------------------------------
void Screen1App::updateFromGui() {
	// ����ģ�Ͳ���
	rotationSpeed = guiRotationSpeed;
	modelScale.set(guiModelScale, guiModelScale, guiModelScale);

	// ���¹��ղ���
	lightingParams.lightIntensity = guiLightIntensity;

	// �ֶ���ת����
	if (!guiAutoRotation) {
		// ����ر��Զ���ת��ֹͣ��ת
		// currentRotationY���ֵ�ǰֵ
	}
}

//--------------------------------------------------------------
void Screen1App::updateRotation() {
	if (guiAutoRotation) {
		currentRotationY += rotationSpeed * ofGetLastFrameTime();
		if (currentRotationY >= 360.0f) {
			currentRotationY -= 360.0f;
		}
	}
}

//--------------------------------------------------------------
void Screen1App::update() {
	elapsedTime = ofGetElapsedTimef();
	dataManager.setElapsedTime(elapsedTime);

	// ��ⴰ�ڴ�С�仯
	static int lastWidth = ofGetWidth();
	static int lastHeight = ofGetHeight();

	if (lastWidth != ofGetWidth() || lastHeight != ofGetHeight()) {
		handleWindowResize(ofGetWidth(), ofGetHeight());
		lastWidth = ofGetWidth();
		lastHeight = ofGetHeight();
	}

	// ��GUI���²���
	updateFromGui();

	// ������ת
	updateRotation();
	renderToPositionTexture();

	 if (isModelLoaded) {
		ofMatrix4x4 modelMatrix = getModelMatrix();
		dataManager.setScreen1ModelMatrix(modelMatrix);
		dataManager.setScreen1Mesh(loadedModel); // ����ʵ��mesh����
		dataManager.setCurrentModelPath(currentModelPath); // ����·�����ڵ���
		// ���������ֻ���һ�Σ�
		static bool debugPrinted = false;
		if (!debugPrinted) {
			ofLogNotice("Screen1App") << "Sharing model data: " << currentModelPath;
			ofLogNotice("Screen1App") << "Model vertices: " << loadedModel.getNumVertices();
			debugPrinted = true;
		}
	}
}

//--------------------------------------------------------------
void Screen1App::handleWindowResize(int w, int h) {
	if (fbo.isAllocated()) {
		fbo.clear();
	}
	fbo.allocate(w, h, GL_RGBA);

	fbo.begin();
	ofClear(20);
	fbo.end();

	cam.setAspectRatio((float)w / (float)h);

	ofLogNotice("Screen1App") << "Window resized to: " << w << "x" << h;
}

//--------------------------------------------------------------
void Screen1App::draw() {
	renderToFBO();
	fbo.draw(0, 0);

	if (showGui) {
		gui.draw();
	}

	if (!showGui) {
		renderUI();
	}
}

//--------------------------------------------------------------
void Screen1App::renderToFBO() {
	fbo.begin();
	ofClear(20, 20, 20, 255); // ���ɫ����

	cam.begin();
	renderModel();
	cam.end();

	fbo.end();
}

//--------------------------------------------------------------
void Screen1App::renderModel() {
	if (!isModelLoaded) {
		// ��ʾ��ʾ����
		ofSetColor(255, 100, 100);
		ofDrawBitmapString("No Model Loaded\nDrag & Drop a model file\nOr check data/models/ folder", -100, 0);
		return;
	}

	ofPushMatrix();

	// Ӧ�ñ任
	ofTranslate(modelPosition);
	ofRotateYDeg(currentRotationY);
	ofScale(modelScale.x, modelScale.y, modelScale.z);

	// ��Ⱦģ��
	if (modelShader.isLoaded()) {
		modelShader.begin();
		setShaderUniforms();

		ofSetColor(255);
		loadedModel.draw();

		modelShader.end();
	} else {
		// ������Ⱦ
		ofSetColor(200, 200, 255);
		loadedModel.draw();

		// �߿���Ⱦ
		ofPushStyle();
		ofNoFill();
		ofSetLineWidth(1.0f);
		ofSetColor(255, 255, 100);
		loadedModel.drawWireframe();
		ofPopStyle();
	}

	ofPopMatrix();

	// ��Դ���ӻ�
	ofPushStyle();
	ofSetColor(255, 255, 100);
	ofVec3f lightPos = calculateLightPosition();
	ofDrawSphere(lightPos, 8.0f);
	ofPopStyle();
}

//--------------------------------------------------------------
void Screen1App::setShaderUniforms() {
	setBasicUniforms();
	setMatrixUniforms();
	setLightingUniforms();
}

//--------------------------------------------------------------
void Screen1App::setBasicUniforms() {
	modelShader.setUniform1f("time", elapsedTime);
}

//--------------------------------------------------------------
void Screen1App::setMatrixUniforms() {
	ofMatrix4x4 modelMatrix = getModelMatrix();
	ofMatrix4x4 modelViewMatrix = cam.getModelViewMatrix();
	ofMatrix4x4 modelViewProjectionMatrix = cam.getModelViewProjectionMatrix();
	ofMatrix4x4 normalMatrix = modelViewMatrix.getInverse();

	modelShader.setUniformMatrix4f("modelMatrix", modelMatrix);
	modelShader.setUniformMatrix4f("modelViewMatrix", modelViewMatrix);
	modelShader.setUniformMatrix4f("modelViewProjectionMatrix", modelViewProjectionMatrix);
	modelShader.setUniformMatrix4f("normalMatrix", normalMatrix);
}

//--------------------------------------------------------------
void Screen1App::setLightingUniforms() {
	ofVec3f lightPos = calculateLightPosition();
	ofVec3f camPos = cam.getPosition();

	modelShader.setUniform3f("lightPosition", lightPos.x, lightPos.y, lightPos.z);
	modelShader.setUniform3f("cameraPosition", camPos.x, camPos.y, camPos.z);
	modelShader.setUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
	modelShader.setUniform3f("ambientColor", 0.2f, 0.2f, 0.2f);
	modelShader.setUniform1f("lightIntensity", lightingParams.lightIntensity);
	modelShader.setUniform1f("shininess", lightingParams.specularShininess);
}

//--------------------------------------------------------------
ofVec3f Screen1App::calculateLightPosition() {
	float lightRadius = 300.0f;
	float lightHeight = 150.0f;
	float lightAngle = elapsedTime * 0.5f;

	return ofVec3f(
		cos(lightAngle) * lightRadius,
		lightHeight + sin(elapsedTime * 0.3f) * 30.0f,
		sin(lightAngle) * lightRadius);
}

//--------------------------------------------------------------
void Screen1App::loadDefaultModel() {
	// ���ȳ��Լ���ָ���Ĳ����ļ�
	string testModelPath = "models/Screen1TestObj.obj";

	if (ofFile::doesFileExist(testModelPath)) {
		ofLogNotice("Screen1App") << "Loading test model: " << testModelPath;
		loadModelFromFile(testModelPath);
		return;
	}

	// ��������ļ������ڣ�������������ģ��
	vector<string> possibleModels = {
		"models/monkey.obj",
		"models/cube.obj",
		"models/sphere.obj",
		"models/teapot.obj"
	};

	bool loaded = false;
	for (const auto & path : possibleModels) {
		if (ofFile::doesFileExist(path)) {
			loadModelFromFile(path);
			loaded = true;
			break;
		}
	}

	// �����û�У�����Ĭ��sphere
	if (!loaded) {
		ofLogNotice("Screen1App") << "No model files found, creating default sphere";
		ofSpherePrimitive sphere;
		sphere.set(80, 32);
		loadedModel = sphere.getMesh();
		isModelLoaded = true;
		currentModelPath = "primitive_sphere";
		ofLogNotice("Screen1App") << "Default sphere created: " << loadedModel.getNumVertices() << " vertices";
	}
}

//--------------------------------------------------------------
void Screen1App::loadModelFromFile(const string & filepath) {
	ofLogNotice("Screen1App") << "Attempting to load model: " << filepath;

	ofVboMesh tempMesh;
	if (modelLoader.loadModel(filepath, tempMesh)) {
		if (validateModel(tempMesh)) {
			loadedModel = tempMesh;
			isModelLoaded = true;
			currentModelPath = filepath;
			ofLogNotice("Screen1App") << "Successfully loaded model: " << filepath;
			ofLogNotice("Screen1App") << "Vertices: " << loadedModel.getNumVertices();
			ofLogNotice("Screen1App") << "Indices: " << loadedModel.getNumIndices();
		} else {
			ofLogError("Screen1App") << "Model validation failed: " << filepath;
		}
	} else {
		ofLogError("Screen1App") << "Failed to load model: " << filepath;
	}
}

//--------------------------------------------------------------
bool Screen1App::validateModel(const ofVboMesh & mesh) {
	if (mesh.getNumVertices() == 0) {
		ofLogError("Screen1App") << "Model has no vertices";
		return false;
	}

	if (mesh.getNumVertices() > 100000) {
		ofLogWarning("Screen1App") << "Large model detected (" << mesh.getNumVertices() << " vertices)";
		// ����ֹ���أ�ֻ�Ǿ���
	}

	return true;
}

//--------------------------------------------------------------
ofMatrix4x4 Screen1App::getModelMatrix() const {
	ofMatrix4x4 matrix;
	matrix.translate(modelPosition);
	matrix.rotateRad(ofDegToRad(currentRotationY), 0, 1, 0);
	matrix.scale(modelScale.x, modelScale.y, modelScale.z);
	return matrix;
}

//--------------------------------------------------------------
void Screen1App::renderUI() {
	ofSetColor(255);
	string info = getDebugInfo();
	ofDrawBitmapString(info, 10, 20);

	// ���ڴ�С��ʾ
	ofSetColor(255, 255, 0);
	string sizeInfo = "Window: " + ofToString(ofGetWidth()) + "x" + ofToString(ofGetHeight())
		+ " | FBO: " + ofToString(fbo.getWidth()) + "x" + ofToString(fbo.getHeight());
	ofDrawBitmapString(sizeInfo, 10, ofGetHeight() - 20);
}

//--------------------------------------------------------------
string Screen1App::getDebugInfo() {
	string info = "FPS: " + ofToString(ofGetFrameRate(), 0) + "\n";
	info += "Model: ";
	if (isModelLoaded) {
		info += "LOADED\n";
		info += "Vertices: " + ofToString(loadedModel.getNumVertices()) + "\n";
		info += "File: " + currentModelPath + "\n";
	} else {
		info += "NONE\n";
	}
	info += "Shader: ";
	if (modelShader.isLoaded()) {
		info += "LOADED\n";
	} else {
		info += "BASIC\n";
	}
	info += "Rotation: " + ofToString(currentRotationY, 1) + " deg\n\n";

	info += "Controls:\n";
	info += "G: Toggle GUI\n";
	info += "F: Toggle Fullscreen\n";
	info += "R: Reset Parameters\n";
	info += "Drag & Drop: Load Model\n";

	return info;
}

//--------------------------------------------------------------
void Screen1App::keyPressed(int key) {
	switch (key) {
	case 'g':
	case 'G':
		showGui = !showGui;
		ofLogNotice() << "GUI: " << (showGui ? "ON" : "OFF");
		break;

	case 'f':
	case 'F':
		ofToggleFullscreen();
		break;

	case 'r':
	case 'R':
		resetAllParameters();
		break;
	}
}

//--------------------------------------------------------------
void Screen1App::dragEvent(ofDragInfo dragInfo) {
	if (dragInfo.files.size() > 0) {
		string filepath = dragInfo.files[0].string();
		string extension = ofToLower(ofFilePath::getFileExt(filepath));

		// ����ļ���չ��
		if (extension == "obj" || extension == "ply" || extension == "3ds" || extension == "dae" || extension == "fbx") {
			loadModelFromFile(filepath);
		} else {
			ofLogWarning("Screen1App") << "Unsupported file format: " << extension;
		}
	}
}

//--------------------------------------------------------------
void Screen1App::resetAllParameters() {
	rotationSpeed = 30.0f;
	currentRotationY = 0.0f;
	modelPosition = ofVec3f(0, 0, 0);
	modelScale = ofVec3f(1, 1, 1);
	lightingParams = LightingParams();

	// ͬ��GUI����
	guiAutoRotation = true;
	guiRotationSpeed = rotationSpeed;
	guiModelScale = 1.0f;
	guiLightIntensity = lightingParams.lightIntensity;

	ofLogNotice() << "Reset all parameters to default";
}

//--------------------------------------------------------------
void Screen1App::logSystemInfo() {
	ofLogNotice("Screen1App") << "=== Model Display System ===";
	ofLogNotice("Screen1App") << "OpenGL Version: " << glGetString(GL_VERSION);
	ofLogNotice("Screen1App") << "Window Size: " << ofGetWidth() << "x" << ofGetHeight();
	ofLogNotice("Screen1App") << "Model Loaded: " << (isModelLoaded ? "Yes" : "No");
	if (isModelLoaded) {
		ofLogNotice("Screen1App") << "Model Path: " << currentModelPath;
		ofLogNotice("Screen1App") << "Vertices: " << loadedModel.getNumVertices();
	}
}
void Screen1App::setupPositionRendering() {
	// ����λ����ȾFBO - ʹ��32λ�����ʽ�洢��ȷλ��
	ofFboSettings fboSettings;
	fboSettings.width = ofGetWidth();
	fboSettings.height = ofGetHeight();
	fboSettings.internalformat = GL_RGBA32F; // RGBA32F��ʽ�洢��������
	fboSettings.useDepth = true;
	fboSettings.depthStencilAsTexture = true; // �����Ϊ����

	positionFBO.allocate(fboSettings);

	// ����λ����Ⱦshader
	if (!positionRenderShader.load("shaders/screen1/position")) {
		ofLogError("Screen1App") << "Failed to load position render shader!";
	} else {
		ofLogNotice("Screen1App") << "Position render shader loaded successfully";
	}
}

void Screen1App::renderToPositionTexture() {
	if (!isModelLoaded || !positionRenderShader.isLoaded()) return;

	positionFBO.begin();
	ofClear(0, 0, 0, 0);

	cam.begin();

	positionRenderShader.begin();

	// �����������
	ofMatrix4x4 modelMatrix = getModelMatrix();
	ofMatrix4x4 viewMatrix = cam.getModelViewMatrix();
	ofMatrix4x4 projectionMatrix = cam.getProjectionMatrix();

	// ��ȷ�ľ�����Ϸ�ʽ
	ofMatrix4x4 modelViewMatrix = viewMatrix * modelMatrix;
	ofMatrix4x4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;

	positionRenderShader.setUniformMatrix4f("modelMatrix", modelMatrix);
	positionRenderShader.setUniformMatrix4f("modelViewMatrix", modelViewMatrix);
	positionRenderShader.setUniformMatrix4f("modelViewProjectionMatrix", modelViewProjectionMatrix);

	// ��Ⱦģ��
	loadedModel.draw();

	positionRenderShader.end();
	cam.end();
	positionFBO.end();

	// ���������DataManager
	dataManager.setScreen1PositionTexture(
		positionFBO.getTexture(),
		positionFBO.getDepthTexture());


	static bool debugPrinted = false;
	if (!debugPrinted) {
		ofLogNotice("Screen1App") << "Position rendering setup - Shader loaded: " << positionRenderShader.isLoaded();
		ofLogNotice("Screen1App") << "Position rendering setup - Model loaded: " << isModelLoaded;
		debugPrinted = true;
	}
}

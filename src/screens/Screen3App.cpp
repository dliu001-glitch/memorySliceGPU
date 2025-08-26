#include "Screen3App.h"

Screen3App::Screen3App()
	: dataManager(DataManager::getInstance()) {
}

//--------------------------------------------------------------
void Screen3App::setup() {
	ofLogNotice("Screen3App") << "Initializing screen space mixing...";

	setupCamera();
	setupShaders();
	setupFBO();
	setupFullScreenQuad();
	setupGui();

	// ��ʼ�����ղ���
	lightingParams = LightingParams();

	logSystemInfo();
}

void Screen3App::setupFullScreenQuad() {
	// ����ȫ���ı���������Ļ�ռ���Ⱦ
	fullScreenQuad.set(2, 2); // ��СΪ2x2������������Ļ
	fullScreenQuad.setPosition(0, 0, 0);
}

void Screen3App::setupShaders() {
	if (!screenSpaceMixShader.load("shaders/screen3/screenSpaceMix")) {
		ofLogError("Screen3App") << "Failed to load screen space mix shader!";
	} else {
		ofLogNotice("Screen3App") << "Screen space mix shader loaded successfully";
	}
}

void Screen3App::setupFBO() {
	finalFBO.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
}

void Screen3App::setupGui() {
	gui.setup("Screen Space Mix Control");
	gui.setDefaultWidth(300);

	// ���ò���
	guiMixRatio.set("Mix Ratio (Screen2->Screen1)", 0.5f, 0.0f, 1.0f);
	guiEnableModel.set("Enable Screen1 Model", true);
	guiEnableGeometry.set("Enable Screen2 Geometry", true);
	guiModelInfluence.set("Model Influence", 1.0f, 0.0f, 2.0f);
	guiGeometryInfluence.set("Geometry Influence", 1.0f, 0.0f, 2.0f);

	guiShowScreen1.set("Show Screen1 Debug", false);
	guiShowScreen2.set("Show Screen2 Debug", false);
	guiMixColor.set("Mix Color", ofColor(255, 255, 255), ofColor(0), ofColor(255));

	// ��ӵ�GUI
	gui.add(guiMixRatio);
	gui.add(guiEnableModel);
	gui.add(guiEnableGeometry);
	gui.add(guiModelInfluence);
	gui.add(guiGeometryInfluence);
	gui.add(guiShowScreen1);
	gui.add(guiShowScreen2);
	gui.add(guiMixColor);
}

void Screen3App::update() {
	elapsedTime = ofGetElapsedTimef();
	dataManager.setElapsedTime(elapsedTime);

	// �����ڴ�С�仯
	static int lastWidth = ofGetWidth();
	static int lastHeight = ofGetHeight();
	if (lastWidth != ofGetWidth() || lastHeight != ofGetHeight()) {
		handleWindowResize(ofGetWidth(), ofGetHeight());
		lastWidth = ofGetWidth();
		lastHeight = ofGetHeight();
	}

	// ����λ����������
	updatePositionTextures();
}

void Screen3App::updatePositionTextures() {
	// ��DataManager��ȡ���µ�λ������
	bool hasScreen1Data = dataManager.hasScreen1PositionData();
	bool hasScreen2Data = dataManager.hasScreen2PositionData();

	if (hasScreen1Data && hasScreen2Data) {
		screen1PosTexture = dataManager.getScreen1PositionTexture();
		screen1DepthTexture = dataManager.getScreen1DepthTexture();
		screen2PosTexture = dataManager.getScreen2PositionTexture();
		screen2DepthTexture = dataManager.getScreen2DepthTexture();

		hasValidTextures = true;

		// ������Ϣ��ֻ���һ�Σ�
		static bool debugPrinted = false;
		if (!debugPrinted) {
			ofLogNotice("Screen3App") << "Received position textures from both screens";
			ofLogNotice("Screen3App") << "Screen1 texture: " << screen1PosTexture.getWidth() << "x" << screen1PosTexture.getHeight();
			ofLogNotice("Screen3App") << "Screen2 texture: " << screen2PosTexture.getWidth() << "x" << screen2PosTexture.getHeight();
			debugPrinted = true;
		}
	} else {
		hasValidTextures = false;
		static bool warningPrinted = false;
		if (!warningPrinted) {
			ofLogWarning("Screen3App") << "Missing position data - Screen1: " << hasScreen1Data << ", Screen2: " << hasScreen2Data;
			warningPrinted = true;
		}
	}
}

void Screen3App::draw() {
	// ��Ⱦ��FBO
	finalFBO.begin();
	ofClear(20, 20, 20, 255);

	if (hasValidTextures && screenSpaceMixShader.isLoaded()) {
		renderScreenSpaceMix();
	} else {
		// ��ʾ������Ϣ
		ofSetColor(255, 100, 100);
		ofDrawBitmapString("Waiting for position textures...", 20, 30);
		string status = "Screen1: " + string(dataManager.hasScreen1PositionData() ? "OK" : "MISSING");
		status += " | Screen2: " + string(dataManager.hasScreen2PositionData() ? "OK" : "MISSING");
		status += " | Shader: " + string(screenSpaceMixShader.isLoaded() ? "OK" : "MISSING");
		ofDrawBitmapString(status, 20, 50);
	}

	finalFBO.end();

	// ��ʾ���ս��
	finalFBO.draw(0, 0);

	if (showGui) {
		gui.draw();
	}

	if (!showGui) {
		renderUI();
	}
}

void Screen3App::renderScreenSpaceMix() {
	// ʹ���ִ�openFrameworks������ͶӰ��ʽ
	ofPushMatrix();

	// ����1��ʹ��ofSetupScreenOrtho
	ofSetupScreenOrtho();

	screenSpaceMixShader.begin();

	// �������������
	screenSpaceMixShader.setUniformTexture("screen1PositionTex", screen1PosTexture, 0);
	screenSpaceMixShader.setUniformTexture("screen1DepthTex", screen1DepthTexture, 1);
	screenSpaceMixShader.setUniformTexture("screen2PositionTex", screen2PosTexture, 2);
	screenSpaceMixShader.setUniformTexture("screen2DepthTex", screen2DepthTexture, 3);

	// ���û�ϲ���
	screenSpaceMixShader.setUniform1f("mixRatio", guiMixRatio.get());
	screenSpaceMixShader.setUniform1i("enableModel", guiEnableModel.get() ? 1 : 0);
	screenSpaceMixShader.setUniform1i("enableGeometry", guiEnableGeometry.get() ? 1 : 0);
	screenSpaceMixShader.setUniform1f("modelInfluence", guiModelInfluence.get());
	screenSpaceMixShader.setUniform1f("geometryInfluence", guiGeometryInfluence.get());

	// ������Ļ����
	screenSpaceMixShader.setUniform2f("resolution", ofGetWidth(), ofGetHeight());
	screenSpaceMixShader.setUniform1f("time", elapsedTime);

	// ���ù��ղ���
	ofVec3f lightPos = calculateLightPosition();
	ofVec3f camPos = cam.getPosition();

	screenSpaceMixShader.setUniform3f("lightPosition", lightPos.x, lightPos.y, lightPos.z);
	screenSpaceMixShader.setUniform3f("cameraPosition", camPos.x, camPos.y, camPos.z);
	screenSpaceMixShader.setUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
	screenSpaceMixShader.setUniform3f("ambientColor", 0.2f, 0.2f, 0.2f);
	screenSpaceMixShader.setUniform1f("lightIntensity", lightingParams.lightIntensity);
	screenSpaceMixShader.setUniform1f("shininess", lightingParams.specularShininess);

	// ���õ���ģʽ
	screenSpaceMixShader.setUniform1i("showScreen1Debug", guiShowScreen1.get() ? 1 : 0);
	screenSpaceMixShader.setUniform1i("showScreen2Debug", guiShowScreen2.get() ? 1 : 0);

	// ��Ⱦȫ�����ζ�����3D�ı���
	ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

	screenSpaceMixShader.end();

	ofPopMatrix();
}
void Screen3App::keyPressed(int key) {
	switch (key) {
	case 'g':
	case 'G':
		showGui = !showGui;
		ofLogNotice() << "GUI: " << (showGui ? "ON" : "OFF");
		break;

	case 'r':
	case 'R':
		resetAllParameters();
		break;

	case '1':
		guiShowScreen1 = !guiShowScreen1;
		ofLogNotice() << "Screen1 Debug: " << (guiShowScreen1.get() ? "ON" : "OFF");
		break;

	case '2':
		guiShowScreen2 = !guiShowScreen2;
		ofLogNotice() << "Screen2 Debug: " << (guiShowScreen2.get() ? "ON" : "OFF");
		break;
	}
}

void Screen3App::handleWindowResize(int w, int h) {
	if (finalFBO.isAllocated()) {
		finalFBO.clear();
	}
	finalFBO.allocate(w, h, GL_RGBA);

	finalFBO.begin();
	ofClear(20);
	finalFBO.end();

	cam.setAspectRatio((float)w / (float)h);
	ofLogNotice("Screen3App") << "Window resized to: " << w << "x" << h;
}

ofVec3f Screen3App::calculateLightPosition() {
	float lightRadius = 350.0f;
	float lightHeight = 150.0f;
	float lightAngle = elapsedTime * 0.4f;

	return ofVec3f(
		cos(lightAngle) * lightRadius,
		lightHeight + sin(elapsedTime * 0.25f) * 40.0f,
		sin(lightAngle) * lightRadius);
}

string Screen3App::getDebugInfo() {
	string info = "=== SCREEN SPACE MIXING DEBUG ===\n";
	info += "FPS: " + ofToString(ofGetFrameRate(), 0) + "\n\n";

	// ����״̬
	info += "=== TEXTURE STATUS ===\n";
	info += "Has Valid Textures: " + string(hasValidTextures ? "YES" : "NO") + "\n";
	info += "Screen1 Data: " + string(dataManager.hasScreen1PositionData() ? "YES" : "NO") + "\n";
	info += "Screen2 Data: " + string(dataManager.hasScreen2PositionData() ? "YES" : "NO") + "\n\n";

	if (hasValidTextures) {
		info += "Screen1 Texture: " + ofToString(screen1PosTexture.getWidth()) + "x" + ofToString(screen1PosTexture.getHeight()) + "\n";
		info += "Screen2 Texture: " + ofToString(screen2PosTexture.getWidth()) + "x" + ofToString(screen2PosTexture.getHeight()) + "\n";
	}

	// ���״̬
	info += "\n=== MIXING STATUS ===\n";
	info += "Mix Ratio: " + ofToString(guiMixRatio.get() * 100, 0) + "%\n";
	info += "Enable Model: " + string(guiEnableModel.get() ? "YES" : "NO") + "\n";
	info += "Enable Geometry: " + string(guiEnableGeometry.get() ? "YES" : "NO") + "\n";
	info += "Shader: " + string(screenSpaceMixShader.isLoaded() ? "LOADED" : "FAILED") + "\n\n";

	info += "Controls:\n";
	info += "G: Toggle GUI\n";
	info += "R: Reset Parameters\n";
	info += "1: Toggle Screen1 Debug\n";
	info += "2: Toggle Screen2 Debug\n";

	return info;
}

void Screen3App::renderUI() {
	ofSetColor(255);
	string info = getDebugInfo();
	ofDrawBitmapString(info, 10, 20);
}

void Screen3App::resetAllParameters() {
	// �������в�����Ĭ��ֵ
	guiMixRatio = 0.5f;
	guiEnableModel = true;
	guiEnableGeometry = true;
	guiModelInfluence = 1.0f;
	guiGeometryInfluence = 1.0f;
	guiShowScreen1 = false;
	guiShowScreen2 = false;
	guiMixColor = ofColor(255, 255, 255);

	// ���ù��ղ���
	lightingParams = LightingParams();

	ofLogNotice() << "Reset all screen space mixing parameters to default";
}

void Screen3App::logSystemInfo() {
	ofLogNotice("Screen3App") << "=== Screen Space Mixing System ===";
	ofLogNotice("Screen3App") << "OpenGL Version: " << glGetString(GL_VERSION);
	ofLogNotice("Screen3App") << "Window Size: " << ofGetWidth() << "x" << ofGetHeight();
	ofLogNotice("Screen3App") << "Final FBO Size: " << finalFBO.getWidth() << "x" << finalFBO.getHeight();
	ofLogNotice("Screen3App") << "Shader Status: " << (screenSpaceMixShader.isLoaded() ? "Loaded" : "Failed");
}
void Screen3App::setupCamera() {
	cam.setDistance(500);
	cam.setNearClip(1.0f);
	cam.setFarClip(2000.0f);
	cam.setPosition(0, 0, 500);
	cam.lookAt(ofVec3f(0, 0, 0));
}

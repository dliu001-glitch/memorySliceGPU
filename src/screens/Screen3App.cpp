#include "Screen3App.h"

Screen3App::Screen3App()
	: dataManager(DataManager::getInstance())
	, tboInitialized(false)
	, hasDrivingMesh(false)
	, showGui(true)
	, screen1PositionTBO(0)
	, screen1PositionTexture(0) {
}

//--------------------------------------------------------------
void Screen3App::setup() {
	ofLogNotice("Screen3App") << "Initializing TBO-based mesh fusion...";

	setupCamera();
	setupShader();
	setupFBO();
	setupGui();
	setupTBO();

	logSystemInfo();
}

//--------------------------------------------------------------
void Screen3App::setupCamera() {
	cam.setDistance(500);
	cam.setNearClip(1.0f);
	cam.setFarClip(2000.0f);
	cam.setPosition(0, 0, 500);
	cam.lookAt(ofVec3f(0, 0, 0));
}

//--------------------------------------------------------------
void Screen3App::setupShader() {
	// Load fusion shader (we'll create this next)
	if (!fusionShader.load("shaders/screen3/fusion")) {
		ofLogError("Screen3App") << "Failed to load fusion shader!";
	} else {
		ofLogNotice("Screen3App") << "Fusion shader loaded successfully";
	}
}

//--------------------------------------------------------------
void Screen3App::setupFBO() {
	finalFBO.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
	finalFBO.begin();
	ofClear(20, 20, 20, 255);
	finalFBO.end();
}

//--------------------------------------------------------------
void Screen3App::setupGui() {
	gui.setup("Mesh Fusion Control");
	gui.setDefaultWidth(300);

	mixRatio.set("Mix Ratio (Screen2->Screen1)", 0.5f, 0.0f, 1.0f);
	enableFusion.set("Enable Fusion", true);
	showDebugInfo.set("Show Debug Info", false);

	gui.add(mixRatio);
	gui.add(enableFusion);
	gui.add(showDebugInfo);
}

//--------------------------------------------------------------
void Screen3App::setupTBO() {
	// Initialize TBO objects
	glGenBuffers(1, &screen1PositionTBO);
	glGenTextures(1, &screen1PositionTexture);

	ofLogNotice("Screen3App") << "TBO objects created: Buffer=" << screen1PositionTBO
							  << " Texture=" << screen1PositionTexture;
}

//--------------------------------------------------------------
void Screen3App::update() {
	// Handle window resize
	static int lastWidth = ofGetWidth();
	static int lastHeight = ofGetHeight();
	if (lastWidth != ofGetWidth() || lastHeight != ofGetHeight()) {
		handleWindowResize(ofGetWidth(), ofGetHeight());
		lastWidth = ofGetWidth();
		lastHeight = ofGetHeight();
	}

	// Update driving mesh from Screen2
	updateDrivingMesh();

	// Update Screen1 position data in TBO
	if (dataManager.hasScreen1MeshData()) {
		updateScreen1TBO();
	}
}

//--------------------------------------------------------------
void Screen3App::updateDrivingMesh() {
	// Use Screen2's mesh as the driving mesh
	if (dataManager.hasScreen2MeshData()) {
		drivingMesh = dataManager.getScreen2BaseMesh();
		hasDrivingMesh = true;

		static bool loggedOnce = false;
		if (!loggedOnce) {
			ofLogNotice("Screen3App") << "Driving mesh updated: "
									  << drivingMesh.getNumVertices() << " vertices";
			loggedOnce = true;
		}
	} else {
		hasDrivingMesh = false;
	}
}

//--------------------------------------------------------------
void Screen3App::updateScreen1TBO() {
	if (!dataManager.hasScreen1MeshData()) return;

	ofVboMesh screen1Mesh = dataManager.getScreen1Mesh();

	// Get vertices as glm::vec3 (OF's current type)
	auto glmVertices = screen1Mesh.getVertices();

	if (glmVertices.empty()) return;

	// Convert to float array for OpenGL
	vector<float> vertexData;
	vertexData.reserve(glmVertices.size() * 3);

	for (const auto & vertex : glmVertices) {
		vertexData.push_back(vertex.x);
		vertexData.push_back(vertex.y);
		vertexData.push_back(vertex.z);
	}

	// Upload to TBO
	glBindBuffer(GL_TEXTURE_BUFFER, screen1PositionTBO);
	glBufferData(GL_TEXTURE_BUFFER, vertexData.size() * sizeof(float),
		vertexData.data(), GL_DYNAMIC_DRAW);

	// Bind texture to buffer
	glBindTexture(GL_TEXTURE_BUFFER, screen1PositionTexture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, screen1PositionTBO);

	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	if (!tboInitialized) {
		ofLogNotice("Screen3App") << "TBO initialized with " << glmVertices.size() << " vertices";
		tboInitialized = true;
	}
}

//--------------------------------------------------------------
void Screen3App::draw() {
	finalFBO.begin();
	ofClear(20, 20, 20, 255);

	if (enableFusion && hasDrivingMesh && tboInitialized && fusionShader.isLoaded()) {
		renderFusion();
	} else {
		// Show status
		ofSetColor(255, 100, 100);
		string status = "Fusion Status:\n";
		status += "Enable: " + string(enableFusion ? "ON" : "OFF") + "\n";
		status += "Driving Mesh: " + string(hasDrivingMesh ? "OK" : "MISSING") + "\n";
		status += "TBO: " + string(tboInitialized ? "OK" : "MISSING") + "\n";
		status += "Shader: " + string(fusionShader.isLoaded() ? "OK" : "MISSING") + "\n";
		ofDrawBitmapString(status, 20, 30);
	}

	finalFBO.end();

	// Draw final result
	finalFBO.draw(0, 0);

	if (showGui) {
		gui.draw();
	}

	if (showDebugInfo) {
		renderUI();
	}
}

//--------------------------------------------------------------
void Screen3App::renderFusion() {
	cam.begin();

	fusionShader.begin();

	// TBO binding
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, screen1PositionTexture);
	fusionShader.setUniform1i("screen1PositionsTBO", 0);

	// Basic parameters
	fusionShader.setUniform1f("mixRatio", mixRatio.get());
	fusionShader.setUniform1f("time", ofGetElapsedTimef());

	// Matrices
	fusionShader.setUniformMatrix4f("modelViewProjectionMatrix", cam.getModelViewProjectionMatrix());
	fusionShader.setUniformMatrix4f("modelViewMatrix", cam.getModelViewMatrix());

	// Lighting
	/*ofVec3f lightPos(200, 200, 400);
	fusionShader.setUniform3f("lightPosition", lightPos.x, lightPos.y, lightPos.z);
	fusionShader.setUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
	fusionShader.setUniform3f("ambientColor", 0.2f, 0.2f, 0.2f);*/

	// Screen2 parameters (get ALL parameters from DataManager)
	if (dataManager.hasScreen2MeshData()) {
		CubeMeshConfig meshConfig = dataManager.getCubeMeshConfig();
		FractureParams fractureParams = dataManager.getFractureParams();
		FlowFieldConfig flowConfig = dataManager.getFlowFieldConfig();

		// Basic deformation parameters
		fusionShader.setUniform1f("noiseScale", meshConfig.noiseScale);
		fusionShader.setUniform1f("noiseStrength", meshConfig.noiseStrength);
		fusionShader.setUniform1f("breathSpeed", meshConfig.breathSpeed);
		fusionShader.setUniform1f("breathAmount", meshConfig.breathAmount);
		fusionShader.setUniform1f("flowFieldStrength", meshConfig.flowFieldStrength);

		// Missing parameters that I need to get from Screen2's GUI
		// You might need to add these to DataManager or pass them directly
		fusionShader.setUniform1f("breathIntensity", 2.0f); // Default value, should come from Screen2
		fusionShader.setUniform1f("breathContrast", 1.0f); // Default value, should come from Screen2

		// Fracture parameters
		fusionShader.setUniform1f("fractureAmount", fractureParams.fractureAmount);
		fusionShader.setUniform1f("fractureScale", fractureParams.fractureScale);
		fusionShader.setUniform1f("explosionRadius", fractureParams.explosionRadius);
		fusionShader.setUniform1f("rotationIntensity", fractureParams.rotationIntensity);
		fusionShader.setUniform1f("separationForce", fractureParams.separationForce);

		// All 8 flow centers
		fusionShader.setUniform3f("flowCenter1", flowConfig.flowCenter1.x, flowConfig.flowCenter1.y, flowConfig.flowCenter1.z);
		fusionShader.setUniform3f("flowCenter2", flowConfig.flowCenter2.x, flowConfig.flowCenter2.y, flowConfig.flowCenter2.z);
		fusionShader.setUniform3f("flowCenter3", flowConfig.flowCenter3.x, flowConfig.flowCenter3.y, flowConfig.flowCenter3.z);
		fusionShader.setUniform3f("flowCenter4", flowConfig.flowCenter4.x, flowConfig.flowCenter4.y, flowConfig.flowCenter4.z);
		fusionShader.setUniform3f("flowCenter5", flowConfig.flowCenter5.x, flowConfig.flowCenter5.y, flowConfig.flowCenter5.z);
		fusionShader.setUniform3f("flowCenter6", flowConfig.flowCenter6.x, flowConfig.flowCenter6.y, flowConfig.flowCenter6.z);
		fusionShader.setUniform3f("flowCenter7", flowConfig.flowCenter7.x, flowConfig.flowCenter7.y, flowConfig.flowCenter7.z);
		fusionShader.setUniform3f("flowCenter8", flowConfig.flowCenter8.x, flowConfig.flowCenter8.y, flowConfig.flowCenter8.z);
	}

	// Enable wireframe rendering
	ofPushStyle();
	ofNoFill();
	ofSetLineWidth(1.5f);

	drivingMesh.drawWireframe(); // Change from draw() to drawWireframe()

	ofPopStyle();
	fusionShader.end();

	glBindTexture(GL_TEXTURE_BUFFER, 0);
	cam.end();
}

//--------------------------------------------------------------
void Screen3App::renderUI() {
	ofSetColor(255);
	ofDrawBitmapString(getDebugInfo(), 10, 20);
}

//--------------------------------------------------------------
string Screen3App::getDebugInfo() {
	string info = "=== MESH FUSION DEBUG ===\n";
	info += "FPS: " + ofToString(ofGetFrameRate(), 0) + "\n";

	if (hasDrivingMesh) {
		info += "Driving Mesh (Screen2): " + ofToString(drivingMesh.getNumVertices()) + " vertices\n";
	}

	if (dataManager.hasScreen1MeshData()) {
		ofVboMesh screen1 = dataManager.getScreen1Mesh();
		info += "Screen1 Mesh: " + ofToString(screen1.getNumVertices()) + " vertices\n";
	}

	info += "Mix Ratio: " + ofToString(mixRatio.get() * 100, 0) + "%\n";
	info += "\nControls:\n";
	info += "G: Toggle GUI\n";
	info += "D: Toggle Debug Info\n";

	return info;
}

//--------------------------------------------------------------
void Screen3App::keyPressed(int key) {
	switch (key) {
	case 'g':
	case 'G':
		showGui = !showGui;
		break;
	case 'd':
	case 'D':
		showDebugInfo = !showDebugInfo;
		break;
	}
}

//--------------------------------------------------------------
void Screen3App::handleWindowResize(int w, int h) {
	if (finalFBO.isAllocated()) {
		finalFBO.clear();
	}
	finalFBO.allocate(w, h, GL_RGBA);
	cam.setAspectRatio((float)w / (float)h);
}

//--------------------------------------------------------------
void Screen3App::cleanupTBO() {
	if (screen1PositionTBO != 0) {
		glDeleteBuffers(1, &screen1PositionTBO);
		screen1PositionTBO = 0;
	}
	if (screen1PositionTexture != 0) {
		glDeleteTextures(1, &screen1PositionTexture);
		screen1PositionTexture = 0;
	}
}

//--------------------------------------------------------------
void Screen3App::logSystemInfo() {
	ofLogNotice("Screen3App") << "=== TBO-Based Mesh Fusion System ===";
	ofLogNotice("Screen3App") << "OpenGL Version: " << glGetString(GL_VERSION);
	ofLogNotice("Screen3App") << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION);
	ofLogNotice("Screen3App") << "Window Size: " << ofGetWidth() << "x" << ofGetHeight();
}

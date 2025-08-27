#include "Screen2App.h"

Screen2App::Screen2App()
	: dataManager(DataManager::getInstance()) {
}

//--------------------------------------------------------------
void Screen2App::setup() {
	ofLogNotice("Screen2App") << "Initializing fracture effect screen...";

	setupCamera();
	setupShaders();
	setupFBO();
	setupDefaultParams();
	setupMesh();
	setupGui();
	setupPositionRendering();
	logSystemInfo();
}

//--------------------------------------------------------------
void Screen2App::setupCamera() {
	cam.setDistance(600);
	cam.setNearClip(1.0f);
	cam.setFarClip(2000.0f);
}

//--------------------------------------------------------------
void Screen2App::setupShaders() {
	if (!fractuteShader.load("shaders/geometry/fracture")) {
		ofLogError("Screen2App") << "Failed to load fracture shader!";
	} else {
		ofLogNotice("Screen2App") << "Fracture shader loaded successfully";
	}
}

//--------------------------------------------------------------
void Screen2App::setupFBO() {
	fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
}

//--------------------------------------------------------------
void Screen2App::setupDefaultParams() {
	// 初始化默认配置
	meshConfig = CubeMeshConfig();
	fractureParams = FractureParams();
	dissipationParams = DissipationParams();
	lightingParams = LightingParams();
	flowFieldConfig = FlowFieldConfig();
}

//--------------------------------------------------------------
void Screen2App::setupMesh() {
	cubeMesh.setup(meshConfig);
}

//--------------------------------------------------------------
void Screen2App::setupGui() {
	gui.setup("Fracture Control Panel");
	gui.setDefaultWidth(300);

	// === 设置参数组 ===

	// 网格和基础动画参数组
	meshGroup.setName("Mesh & Animation");
	meshGroup.add(guiNoiseStrength.set("Noise Strength", meshConfig.noiseStrength, 0.0f, 100.0f));
	meshGroup.add(guiBreathAmount.set("Breath Amount", meshConfig.breathAmount, 0.0f, 200.0f));
	meshGroup.add(guiBreathSpeed.set("Breath Speed", meshConfig.breathSpeed, 0.1f, 5.0f));
	meshGroup.add(guiBreathIntensity.set("Breath Intensity", 2.0f, 0.5f, 10.0f));
	meshGroup.add(guiBreathContrast.set("Breath Contrast", 1.0f, 0.1f, 3.0f));
	meshGroup.add(guiFlowFieldStrength.set("Flow Field Strength", meshConfig.flowFieldStrength, 0.0f, 500.0f));
	meshGroup.add(guiGridResolution.set("Grid Resolution", meshConfig.gridResolution, 10, 200));
	meshGroup.add(guiCubeSize.set("Cube Size", meshConfig.cubeSize, 50.0f, 500.0f));
	

	// 破碎效果参数组
	fractureGroup.setName("Fracture Effects");
	fractureGroup.add(guiEnableFracture.set("Enable Fracture", fractureParams.enableFracture));
	fractureGroup.add(guiFractureAmount.set("Fracture Amount", fractureParams.fractureAmount, 0.0f, 2.0f));
	fractureGroup.add(guiFractureScale.set("Fracture Scale", fractureParams.fractureScale, 0.001f, 0.1f));
	fractureGroup.add(guiExplosionRadius.set("Explosion Radius", fractureParams.explosionRadius, 50.0f, 500.0f));
	fractureGroup.add(guiRotationIntensity.set("Rotation Intensity", fractureParams.rotationIntensity, 0.0f, 3.0f));
	fractureGroup.add(guiSeparationForce.set("Separation Force", fractureParams.separationForce, 0.0f, 200.0f));

	// 消散效果参数组
	dissipationGroup.setName("Dissipation Effects");
	dissipationGroup.add(guiEnableDissipation.set("Enable Dissipation", dissipationParams.enableDissipation));
	dissipationGroup.add(guiDissipationAmount.set("Dissipation Amount", dissipationParams.dissipationAmount, 0.0f, 1.0f));
	dissipationGroup.add(guiDissipationScale.set("Dissipation Scale", dissipationParams.dissipationScale, 0.01f, 0.2f));
	dissipationGroup.add(guiDissipationSpeed.set("Dissipation Speed", dissipationParams.dissipationSpeed, 0.1f, 5.0f));
	dissipationGroup.add(guiCloudThreshold.set("Cloud Threshold", dissipationParams.cloudThreshold, 0.0f, 1.0f));
	dissipationGroup.add(guiEdgeSoftness.set("Edge Softness", dissipationParams.edgeSoftness, 0.01f, 0.5f));

	// 光照参数组
	lightingGroup.setName("Lighting");
	lightingGroup.add(guiLightIntensity.set("Light Intensity", lightingParams.lightIntensity, 0.0f, 3.0f));
	lightingGroup.add(guiAmbientStrength.set("Ambient Strength", lightingParams.ambientStrength, 0.0f, 1.0f));
	lightingGroup.add(guiSpecularShininess.set("Specular Shininess", lightingParams.specularShininess, 1.0f, 128.0f));
	lightingGroup.add(guiAutoLightRotation.set("Auto Light Rotation", lightingParams.autoLightRotation));
	lightingGroup.add(guiManualLightAngle.set("Manual Light Angle", lightingParams.manualLightAngle, 0.0f, TWO_PI));

	// 将参数组添加到GUI面板
	gui.add(meshGroup);
	gui.add(fractureGroup);
	gui.add(dissipationGroup);
	gui.add(lightingGroup);
}

//--------------------------------------------------------------
void Screen2App::updateFromGui() {
	// 更新网格配置
	bool needRegenerateMesh = false;
	if (meshConfig.gridResolution != guiGridResolution.get() || meshConfig.cubeSize != guiCubeSize.get()) {
		needRegenerateMesh = true;
	}

	// 同步网格参数
	meshConfig.noiseStrength = guiNoiseStrength;
	meshConfig.breathAmount = guiBreathAmount;
	meshConfig.breathSpeed = guiBreathSpeed;
	meshConfig.flowFieldStrength = guiFlowFieldStrength;
	meshConfig.gridResolution = guiGridResolution;
	meshConfig.cubeSize = guiCubeSize;

	

	if (needRegenerateMesh) {
		cubeMesh.updateConfig(meshConfig);
	}

	// 同步破碎参数
	fractureParams.enableFracture = guiEnableFracture;
	fractureParams.fractureAmount = guiFractureAmount;
	fractureParams.fractureScale = guiFractureScale;
	fractureParams.explosionRadius = guiExplosionRadius;
	fractureParams.rotationIntensity = guiRotationIntensity;
	fractureParams.separationForce = guiSeparationForce;

	// 同步消散参数
	dissipationParams.enableDissipation = guiEnableDissipation;
	dissipationParams.dissipationAmount = guiDissipationAmount;
	dissipationParams.dissipationScale = guiDissipationScale;
	dissipationParams.dissipationSpeed = guiDissipationSpeed;
	dissipationParams.cloudThreshold = guiCloudThreshold;
	dissipationParams.edgeSoftness = guiEdgeSoftness;

	// 同步光照参数
	lightingParams.lightIntensity = guiLightIntensity;
	lightingParams.ambientStrength = guiAmbientStrength;
	lightingParams.specularShininess = guiSpecularShininess;
	lightingParams.autoLightRotation = guiAutoLightRotation;
	lightingParams.manualLightAngle = guiManualLightAngle;
}

//--------------------------------------------------------------
void Screen2App::update() {
	elapsedTime = ofGetElapsedTimef();
	dataManager.setElapsedTime(elapsedTime);

	// 检测窗口大小变化
	static int lastWidth = ofGetWidth();
	static int lastHeight = ofGetHeight();

	if (lastWidth != ofGetWidth() || lastHeight != ofGetHeight()) {
		handleWindowResize(ofGetWidth(), ofGetHeight());
		lastWidth = ofGetWidth();
		lastHeight = ofGetHeight();
	}

	// 根据cubeSize动态调整流场中心点的位置
	float scaleFactor = meshConfig.cubeSize / 200.0f; // 200.0f是默认cubeSize

	flowFieldConfig.flowCenter1 = ofVec3f(80.0f, 80.0f, 80.0f) * scaleFactor;
	flowFieldConfig.flowCenter2 = ofVec3f(-80.0f, -80.0f, 80.0f) * scaleFactor;
	flowFieldConfig.flowCenter3 = ofVec3f(80.0f, 80.0f, -80.0f) * scaleFactor;
	flowFieldConfig.flowCenter4 = ofVec3f(-80.0f, 80.0f, -80.0f) * scaleFactor;
	flowFieldConfig.flowCenter5 = ofVec3f(0.0f, 0.0f, 100.0f) * scaleFactor;
	flowFieldConfig.flowCenter6 = ofVec3f(100.0f, 0.0f, 0.0f) * scaleFactor;
	flowFieldConfig.flowCenter7 = ofVec3f(-100.0f, 0.0f, 0.0f) * scaleFactor;
	flowFieldConfig.flowCenter8 = ofVec3f(0.0f, 100.0f, 0.0f) * scaleFactor;

	// 从GUI更新参数
	updateFromGui();

	renderToPositionTexture();

	// === 关键：将Screen2的参数实时共享给DataManager ===
	dataManager.setCubeMeshConfig(meshConfig);
	dataManager.setFractureParams(fractureParams);
	dataManager.setDissipationParams(dissipationParams);
	dataManager.setLightingParams(lightingParams);
	dataManager.setFlowFieldConfig(flowFieldConfig);

	// Share mesh data with DataManager for Screen3
	dataManager.setScreen2BaseMesh(cubeMesh.getMesh());

	// Add debug output to verify sharing
	static int frameCount = 0;
	if (frameCount++ % 120 == 0) { // Every 2 seconds
		ofLogNotice("Screen2App") << "Sharing mesh with " << cubeMesh.getMesh().getNumVertices() << " vertices";
	}
}

//--------------------------------------------------------------
void Screen2App::handleWindowResize(int w, int h) {
	if (fbo.isAllocated()) {
		fbo.clear();
	}
	fbo.allocate(w, h, GL_RGBA);

	fbo.begin();
	ofClear(20);
	fbo.end();

	cam.setAspectRatio((float)w / (float)h);

	ofLogNotice("Screen2App") << "Window resized to: " << w << "x" << h;
}

//--------------------------------------------------------------
void Screen2App::draw() {
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
void Screen2App::renderToFBO() {
	fbo.begin();
	ofClear(20);

	cam.begin();
	renderGeometry();
	cam.end();

	fbo.end();
}

//--------------------------------------------------------------
void Screen2App::renderGeometry() {
	if (!fractuteShader.isLoaded()) {
		ofSetColor(255, 100, 100);
		cubeMesh.getMesh().drawWireframe();
		return;
	}

	fractuteShader.begin();
	setShaderUniforms();

	ofPushStyle();
	ofNoFill();
	ofSetLineWidth(1.5f);
	cubeMesh.getMesh().drawWireframe();
	ofPopStyle();

	fractuteShader.end();

	// 光源可视化
	ofPushStyle();
	ofSetColor(255, 255, 100);
	ofVec3f lightPos = calculateLightPosition();
	ofDrawSphere(lightPos, 6.0f);
	ofPopStyle();
}

//--------------------------------------------------------------
void Screen2App::setShaderUniforms() {
	setBasicUniforms();
	setMatrixUniforms();
	setLightingUniforms();
	setEffectUniforms();
}

//--------------------------------------------------------------
void Screen2App::setBasicUniforms() {
	fractuteShader.setUniform1f("time", elapsedTime);
	fractuteShader.setUniform1f("noiseScale", meshConfig.noiseScale);
	fractuteShader.setUniform1f("noiseStrength", meshConfig.noiseStrength);
	fractuteShader.setUniform1f("breathSpeed", meshConfig.breathSpeed);
	fractuteShader.setUniform1f("breathAmount", meshConfig.breathAmount);
	fractuteShader.setUniform1f("flowFieldStrength", meshConfig.flowFieldStrength);

	fractuteShader.setUniform1f("breathIntensity", guiBreathIntensity.get());
	fractuteShader.setUniform1f("breathContrast", guiBreathContrast.get());
}

//--------------------------------------------------------------
void Screen2App::setMatrixUniforms() {
	ofMatrix4x4 modelViewProjectionMatrix = cam.getModelViewProjectionMatrix();
	ofMatrix4x4 modelViewMatrix = cam.getModelViewMatrix();
	ofMatrix4x4 normalMatrix = modelViewMatrix.getInverse();

	fractuteShader.setUniformMatrix4f("modelViewProjectionMatrix", modelViewProjectionMatrix);
	fractuteShader.setUniformMatrix4f("modelViewMatrix", modelViewMatrix);
	fractuteShader.setUniformMatrix4f("normalMatrix", normalMatrix);
}

//--------------------------------------------------------------
void Screen2App::setLightingUniforms() {
	ofVec3f lightPos = calculateLightPosition();
	ofVec3f camPos = cam.getPosition();

	fractuteShader.setUniform3f("lightPosition", lightPos.x, lightPos.y, lightPos.z);
	fractuteShader.setUniform3f("cameraPosition", camPos.x, camPos.y, camPos.z);
	fractuteShader.setUniform3f("lightColor", 1.0f, 1.0f, 1.0f);
	fractuteShader.setUniform3f("ambientColor", 0.2f, 0.2f, 0.2f);
	fractuteShader.setUniform1f("lightIntensity", lightingParams.lightIntensity);
	fractuteShader.setUniform1f("shininess", lightingParams.specularShininess);
}

//--------------------------------------------------------------
void Screen2App::setEffectUniforms() {
	fractuteShader.setUniform3f("flowCenter1", flowFieldConfig.flowCenter1.x, flowFieldConfig.flowCenter1.y, flowFieldConfig.flowCenter1.z);
	fractuteShader.setUniform3f("flowCenter2", flowFieldConfig.flowCenter2.x, flowFieldConfig.flowCenter2.y, flowFieldConfig.flowCenter2.z);
	fractuteShader.setUniform3f("flowCenter3", flowFieldConfig.flowCenter3.x, flowFieldConfig.flowCenter3.y, flowFieldConfig.flowCenter3.z);
	fractuteShader.setUniform3f("flowCenter4", flowFieldConfig.flowCenter4.x, flowFieldConfig.flowCenter4.y, flowFieldConfig.flowCenter4.z);
	fractuteShader.setUniform3f("flowCenter5", flowFieldConfig.flowCenter5.x, flowFieldConfig.flowCenter5.y, flowFieldConfig.flowCenter5.z);
	fractuteShader.setUniform3f("flowCenter6", flowFieldConfig.flowCenter6.x, flowFieldConfig.flowCenter6.y, flowFieldConfig.flowCenter6.z);
	fractuteShader.setUniform3f("flowCenter7", flowFieldConfig.flowCenter7.x, flowFieldConfig.flowCenter7.y, flowFieldConfig.flowCenter7.z);
	fractuteShader.setUniform3f("flowCenter8", flowFieldConfig.flowCenter8.x, flowFieldConfig.flowCenter8.y, flowFieldConfig.flowCenter8.z);

	fractuteShader.setUniform1f("fractureAmount", fractureParams.enableFracture ? fractureParams.fractureAmount : 0.0f);
	fractuteShader.setUniform1f("fractureScale", fractureParams.fractureScale);
	fractuteShader.setUniform1f("explosionRadius", fractureParams.explosionRadius);
	fractuteShader.setUniform1f("rotationIntensity", fractureParams.rotationIntensity);
	fractuteShader.setUniform1f("separationForce", fractureParams.separationForce);

	fractuteShader.setUniform1f("dissipationAmount", dissipationParams.enableDissipation ? dissipationParams.dissipationAmount : 0.0f);
	fractuteShader.setUniform1f("dissipationScale", dissipationParams.dissipationScale);
	fractuteShader.setUniform1f("dissipationSpeed", dissipationParams.dissipationSpeed);
	fractuteShader.setUniform1f("cloudThreshold", dissipationParams.cloudThreshold);
	fractuteShader.setUniform1f("edgeSoftness", dissipationParams.edgeSoftness);
}

//--------------------------------------------------------------
ofVec3f Screen2App::calculateLightPosition() {
	float lightRadius = 400.0f;
	float lightHeight = 200.0f;
	float lightAngle = elapsedTime * 0.5f;

	return ofVec3f(
		cos(lightAngle) * lightRadius,
		lightHeight + sin(elapsedTime * 0.3f) * 50.0f,
		sin(lightAngle) * lightRadius);
}

//--------------------------------------------------------------
void Screen2App::renderUI() {
	ofSetColor(255);
	string info = getDebugInfo();
	ofDrawBitmapString(info, 10, 20);
}

//--------------------------------------------------------------
string Screen2App::getDebugInfo() {
	string info = "FPS: " + ofToString(ofGetFrameRate(), 0) + "\n";
	info += "Vertices: " + ofToString(cubeMesh.getVertexCount()) + "\n";
	info += "Shader: " + string(fractuteShader.isLoaded() ? "LOADED" : "FAILED") + "\n\n";

	info += "=== CURRENT EFFECTS ===\n";
	info += "Dissipation: " + string(dissipationParams.enableDissipation ? "ON" : "OFF");
	if (dissipationParams.enableDissipation) {
		info += " (" + ofToString(dissipationParams.dissipationAmount, 2) + ")";
	}
	info += "\n";

	info += "Fracture: " + string(fractureParams.enableFracture ? "ON" : "OFF");
	if (fractureParams.enableFracture) {
		info += " (" + ofToString(fractureParams.fractureAmount, 2) + ")";
	}
	info += "\n\n";

	info += "Controls:\n";
	info += "G: Toggle GUI\n";
	info += "R: Reset Parameters\n";

	return info;
}

//--------------------------------------------------------------
void Screen2App::keyPressed(int key) {
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
	}
}

//--------------------------------------------------------------
void Screen2App::resetAllParameters() {
	meshConfig = CubeMeshConfig();
	fractureParams = FractureParams();
	dissipationParams = DissipationParams();
	lightingParams = LightingParams();
	flowFieldConfig = FlowFieldConfig();

	// 同步所有GUI参数
	guiNoiseStrength = meshConfig.noiseStrength;
	guiBreathAmount = meshConfig.breathAmount;
	guiFlowFieldStrength = meshConfig.flowFieldStrength;
	guiGridResolution = meshConfig.gridResolution;
	guiCubeSize = meshConfig.cubeSize;

	guiEnableFracture = fractureParams.enableFracture;
	guiFractureAmount = fractureParams.fractureAmount;
	guiFractureScale = fractureParams.fractureScale;
	guiExplosionRadius = fractureParams.explosionRadius;
	guiRotationIntensity = fractureParams.rotationIntensity;
	guiSeparationForce = fractureParams.separationForce;

	guiEnableDissipation = dissipationParams.enableDissipation;
	guiDissipationAmount = dissipationParams.dissipationAmount;
	guiDissipationScale = dissipationParams.dissipationScale;
	guiDissipationSpeed = dissipationParams.dissipationSpeed;
	guiCloudThreshold = dissipationParams.cloudThreshold;
	guiEdgeSoftness = dissipationParams.edgeSoftness;

	guiLightIntensity = lightingParams.lightIntensity;
	guiAmbientStrength = lightingParams.ambientStrength;
	guiSpecularShininess = lightingParams.specularShininess;
	guiAutoLightRotation = lightingParams.autoLightRotation;
	guiManualLightAngle = lightingParams.manualLightAngle;

	ofLogNotice() << "Reset all parameters to default";
}

//--------------------------------------------------------------
void Screen2App::logSystemInfo() {
	ofLogNotice("Screen2App") << "=== System Information ===";
	ofLogNotice("Screen2App") << "OpenGL Version: " << glGetString(GL_VERSION);
	ofLogNotice("Screen2App") << "Window Size: " << ofGetWidth() << "x" << ofGetHeight();
	cubeMesh.logMeshInfo();
}
void Screen2App::setupPositionRendering() {
	// 创建位置渲染FBO
	ofFboSettings fboSettings;
	fboSettings.width = ofGetWidth();
	fboSettings.height = ofGetHeight();
	fboSettings.internalformat = GL_RGBA32F; // 32位浮点精度
	fboSettings.useDepth = true;
	fboSettings.depthStencilAsTexture = true;

	positionFBO.allocate(fboSettings);

	// 加载位置渲染shader（复用几何变形逻辑）
	if (!positionRenderShader.load("shaders/screen2/position")) {
		ofLogError("Screen2App") << "Failed to load position render shader!";
	} else {
		ofLogNotice("Screen2App") << "Position render shader loaded successfully";
	}
}

void Screen2App::renderToPositionTexture() {
	if (!positionRenderShader.isLoaded()) return;

	positionFBO.begin();
	ofClear(0, 0, 0, 0); // 透明背景

	cam.begin();
	positionRenderShader.begin();

	// 设置基础矩阵
	ofMatrix4x4 modelMatrix; // 单位矩阵
	ofMatrix4x4 modelViewMatrix = cam.getModelViewMatrix();
	ofMatrix4x4 modelViewProjectionMatrix = cam.getModelViewProjectionMatrix();

	positionRenderShader.setUniformMatrix4f("modelMatrix", modelMatrix);
	positionRenderShader.setUniformMatrix4f("modelViewMatrix", modelViewMatrix);
	positionRenderShader.setUniformMatrix4f("modelViewProjectionMatrix", modelViewProjectionMatrix);

	// 设置几何变形参数
	positionRenderShader.setUniform1f("time", elapsedTime);
	positionRenderShader.setUniform1f("noiseScale", meshConfig.noiseScale);
	positionRenderShader.setUniform1f("noiseStrength", meshConfig.noiseStrength);
	positionRenderShader.setUniform1f("breathSpeed", meshConfig.breathSpeed);
	positionRenderShader.setUniform1f("breathAmount", meshConfig.breathAmount);
	positionRenderShader.setUniform1f("flowFieldStrength", meshConfig.flowFieldStrength);

	// 设置流场中心点
	positionRenderShader.setUniform3f("flowCenter1", flowFieldConfig.flowCenter1.x, flowFieldConfig.flowCenter1.y, flowFieldConfig.flowCenter1.z);
	positionRenderShader.setUniform3f("flowCenter2", flowFieldConfig.flowCenter2.x, flowFieldConfig.flowCenter2.y, flowFieldConfig.flowCenter2.z);
	positionRenderShader.setUniform3f("flowCenter3", flowFieldConfig.flowCenter3.x, flowFieldConfig.flowCenter3.y, flowFieldConfig.flowCenter3.z);
	positionRenderShader.setUniform3f("flowCenter4", flowFieldConfig.flowCenter4.x, flowFieldConfig.flowCenter4.y, flowFieldConfig.flowCenter4.z);
	positionRenderShader.setUniform3f("flowCenter5", flowFieldConfig.flowCenter5.x, flowFieldConfig.flowCenter5.y, flowFieldConfig.flowCenter5.z);
	positionRenderShader.setUniform3f("flowCenter6", flowFieldConfig.flowCenter6.x, flowFieldConfig.flowCenter6.y, flowFieldConfig.flowCenter6.z);
	positionRenderShader.setUniform3f("flowCenter7", flowFieldConfig.flowCenter7.x, flowFieldConfig.flowCenter7.y, flowFieldConfig.flowCenter7.z);
	positionRenderShader.setUniform3f("flowCenter8", flowFieldConfig.flowCenter8.x, flowFieldConfig.flowCenter8.y, flowFieldConfig.flowCenter8.z);

	// 设置破碎效果参数
	positionRenderShader.setUniform1f("fractureAmount", fractureParams.enableFracture ? fractureParams.fractureAmount : 0.0f);
	positionRenderShader.setUniform1f("fractureScale", fractureParams.fractureScale);
	positionRenderShader.setUniform1f("explosionRadius", fractureParams.explosionRadius);
	positionRenderShader.setUniform1f("rotationIntensity", fractureParams.rotationIntensity);
	positionRenderShader.setUniform1f("separationForce", fractureParams.separationForce);

	// 设置消散效果参数
	positionRenderShader.setUniform1f("dissipationAmount", dissipationParams.enableDissipation ? dissipationParams.dissipationAmount : 0.0f);
	positionRenderShader.setUniform1f("dissipationScale", dissipationParams.dissipationScale);
	positionRenderShader.setUniform1f("dissipationSpeed", dissipationParams.dissipationSpeed);
	positionRenderShader.setUniform1f("cloudThreshold", dissipationParams.cloudThreshold);
	positionRenderShader.setUniform1f("edgeSoftness", dissipationParams.edgeSoftness);

	// 渲染几何体
	cubeMesh.getMesh().draw();

	positionRenderShader.end();
	cam.end();
	positionFBO.end();
}

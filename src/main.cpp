#include "ofAppGLFWWindow.h"
#include "ofMain.h"
#include "screens/Screen1App.h"
#include "screens/Screen2App.h"
#include "screens/Screen3App.h"

int main() {
	ofLogNotice("main") << "Starting simplified multi-window application...";

	// === 窗口1：模型显示屏幕 ===
	ofGLFWWindowSettings settings1;
	settings1.setSize(1024, 768);
	settings1.setPosition(glm::vec2(100, 100));
	settings1.resizable = true;
	auto window1 = ofCreateWindow(settings1);

	// === 窗口2：立方体特效屏幕 ===
	ofGLFWWindowSettings settings2;
	settings2.setSize(1024, 768);
	settings2.setPosition(glm::vec2(1150, 100));
	settings2.resizable = true;
	settings2.shareContextWith = window1; // 上下文共享
	auto window2 = ofCreateWindow(settings2);

	// === 窗口3：混合效果屏幕 ===
	ofGLFWWindowSettings settings3;
	settings3.setSize(1024, 768);
	settings3.setPosition(glm::vec2(625, 900));
	settings3.resizable = true;
	settings3.shareContextWith = window1; // 上下文共享
	auto window3 = ofCreateWindow(settings3);

	// 创建应用实例
	auto screen1App = std::make_shared<Screen1App>();
	auto screen2App = std::make_shared<Screen2App>();
	auto screen3App = std::make_shared<Screen3App>();

	// No FBO setup needed - Screen3App gets data through DataManager

	// 运行应用
	ofRunApp(window1, screen1App);
	ofRunApp(window2, screen2App);
	ofRunApp(window3, screen3App);

	ofRunMainLoop();
	return 0;
}

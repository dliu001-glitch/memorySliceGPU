#include "ofAppGLFWWindow.h"
#include "ofMain.h"
#include "screens/Screen1App.h"
#include "screens/Screen2App.h"
#include "screens/Screen3App.h"

int main() {
	ofLogNotice("main") << "Starting simplified multi-window application...";

	// === ����1��ģ����ʾ��Ļ ===
	ofGLFWWindowSettings settings1;
	settings1.setSize(1024, 768);
	settings1.setPosition(glm::vec2(100, 100));
	settings1.resizable = true;
	auto window1 = ofCreateWindow(settings1);

	// === ����2����������Ч��Ļ ===
	ofGLFWWindowSettings settings2;
	settings2.setSize(1024, 768);
	settings2.setPosition(glm::vec2(1150, 100));
	settings2.resizable = true;
	settings2.shareContextWith = window1; // �����Ĺ���
	auto window2 = ofCreateWindow(settings2);

	// === ����3�����Ч����Ļ ===
	ofGLFWWindowSettings settings3;
	settings3.setSize(1024, 768);
	settings3.setPosition(glm::vec2(625, 900));
	settings3.resizable = true;
	settings3.shareContextWith = window1; // �����Ĺ���
	auto window3 = ofCreateWindow(settings3);

	// ����Ӧ��ʵ��
	auto screen1App = std::make_shared<Screen1App>();
	auto screen2App = std::make_shared<Screen2App>();
	auto screen3App = std::make_shared<Screen3App>();

	// No FBO setup needed - Screen3App gets data through DataManager

	// ����Ӧ��
	ofRunApp(window1, screen1App);
	ofRunApp(window2, screen2App);
	ofRunApp(window3, screen3App);

	ofRunMainLoop();
	return 0;
}

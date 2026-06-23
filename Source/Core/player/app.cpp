#include "app.h"

#include "player/canvas.h"


using namespace std;

App::App(AppFactory* factory, const string& app) :
	mod(factory->CreateApp(app)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()) {

}

App::~App() {
	factory->DestroyApp(mod);
}

string App::GetType() const {
	return type;
}

string App::GetName() const {
	return name;
}

void App::Init() const {
	factory->InitApp(type);
}

int App::Loop(Canvas* canvas, int ms) const {
	return factory->LoopApp(type, canvas, ms);
}

void App::Back(Canvas* canvas) const {
	factory->BackApp(type, canvas);
}

void App::Refresh(Canvas* canvas) const {
	factory->RefreshApp(type, canvas);
}

int EmptyApp::count = 0;

EmptyApp::EmptyApp() : id(count++) {

}

EmptyApp::~EmptyApp() {

}

const char* EmptyApp::GetId() {
	return "empty";
}

const char* EmptyApp::GetType() const {
	return "empty";
}

const char* EmptyApp::GetName() {
	name = "空应用" + to_string(id);
	return name.data();
}

void EmptyApp::Init() {

}

int EmptyApp::Loop(Canvas* canvas, int ms) {
	return 0;
}

void EmptyApp::Back(Canvas* canvas) {

}

void EmptyApp::Refresh(Canvas* canvas) {
	if (!canvas) return;
	canvas->ClearBuffer();
}

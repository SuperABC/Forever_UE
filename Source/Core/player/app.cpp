#include "app.h"

#include "player/canvas.h"


using namespace std;

App::App(AppFactory* factory, const string& app) :
	mod(factory->CreateApp(app)),
	factory(factory),
	type(),
	name() {
	if (!mod)
		THROW_EXCEPTION(NullPointerException, "App " + app + " mod is null.\n");

	type = mod->GetType();
	name = mod->GetName();
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

void App::Init(PostHandle* post) const {
	factory->InitApp(type, post);
}

int App::Loop(Canvas* canvas, int ms, PostHandle* post) const {
	return factory->LoopApp(type, canvas, ms, post);
}

void App::Back(Canvas* canvas, PostHandle* post) const {
	factory->BackApp(type, canvas, post);
}

void App::Refresh(Canvas* canvas, PostHandle* post) const {
	factory->RefreshApp(type, canvas, post);
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

void EmptyApp::Init(PostHandle* post) {

}

int EmptyApp::Loop(Canvas* canvas, int ms, PostHandle* post) {
	return 0;
}

void EmptyApp::Back(Canvas* canvas, PostHandle* post) {

}

void EmptyApp::Refresh(Canvas* canvas, PostHandle* post) {
	if (!canvas) return;
	canvas->ClearBuffer();
}

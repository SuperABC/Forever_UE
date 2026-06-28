#include "puzzle.h"


using namespace std;

Puzzle::Puzzle(PuzzleFactory* factory, const string& puzzle) :
	mod(factory->CreatePuzzle(puzzle)),
	factory(factory),
	type(),
	name() {
	if (!mod)
		THROW_EXCEPTION(NullPointerException, "Puzzle " + puzzle + " mod is null.\n");

	type = mod->GetType();
	name = mod->GetName();
}

Puzzle::~Puzzle() {
	factory->DestroyPuzzle(mod);
}

string Puzzle::GetType() const {
	return type;
}

string Puzzle::GetName() const {
	return name;
}

void Puzzle::Init(int width, int height) const {
	factory->InitPuzzle(type, width, height);
}

int Puzzle::Loop(Canvas* canvas, int ms) const {
	return factory->LoopPuzzle(type, canvas, ms);
}

int EmptyPuzzle::count = 0;

EmptyPuzzle::EmptyPuzzle() : id(count++) {

}

EmptyPuzzle::~EmptyPuzzle() {

}

const char* EmptyPuzzle::GetId() {
	return "empty";
}

const char* EmptyPuzzle::GetType() const {
	return "empty";
}

const char* EmptyPuzzle::GetName() {
	name = "空小游戏" + to_string(id);
	return name.data();
}

void EmptyPuzzle::Init(int width, int height) {

}

int EmptyPuzzle::Loop(Canvas* canvas, int ms) {
	return 0;
}

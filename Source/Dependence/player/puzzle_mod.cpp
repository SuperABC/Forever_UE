#include "puzzle_mod.h"


using namespace std;

PuzzleMod::PuzzleMod() {

}

PuzzleMod::~PuzzleMod() {

}

void PuzzleMod::Init(int width, int height, PostHandle* post) {

}

int PuzzleMod::Loop(Canvas* canvas, int ms, PostHandle* post) {
	return 0;
}

PuzzleFactory::PuzzleFactory()
	: registries(),
	configs(),
	temp() {
	temp.registries.reserve(TEMP_RESERVE_CAPACITY);
}

void PuzzleFactory::RegisterPuzzle(const string& id,
	function<void(int, int, PostHandle*)> init, function<int(Canvas*, int, PostHandle*)> loop,
	function<PuzzleMod* ()> creator, function<void(PuzzleMod*)> deleter) {
	temp.registries[id] = { init, loop, creator, deleter };
}

void PuzzleFactory::MergeTemp() {
	for (auto& [id, registry] : temp.registries) {
		registries[id] = registry;
	}
}

void PuzzleFactory::CleanTemp() {
	temp.registries.clear();
}

void PuzzleFactory::RemoveAll() {
	for (auto& [_, enabled] : configs) {
		enabled = false;
	}
}

PuzzleMod* PuzzleFactory::CreatePuzzle(const string& id) const {
	auto config = configs.find(id);
	if (config == configs.end() || !config->second) {
		debugf("Warning: Puzzle %s not enabled when creating.\n", id.data());
		return nullptr;
	}

	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Puzzle %s not registered when creating.\n", id.data());
		return nullptr;
	}

	if (it->second.creator) {
		return it->second.creator();
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Puzzle " + id + " creator is null.\n");
	}

	return nullptr;
}

bool PuzzleFactory::CheckRegistered(const string& id) const {
	return registries.find(id) != registries.end();
}

void PuzzleFactory::SetConfig(const string& name, bool config) {
	configs[name] = config;
}

void PuzzleFactory::InitPuzzle(const string& id, int width, int height, PostHandle* post) const {
	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Puzzle %s not registered when initializing.\n", id.data());
		return;
	}

	if (it->second.init) {
		it->second.init(width, height, post);
	}
	else {
		debugf("Warning: Puzzle %s has no init function.\n", id.data());
	}
}

int PuzzleFactory::LoopPuzzle(const string& id, Canvas* canvas, int ms, PostHandle* post) const {
	auto it = registries.find(id);
	if (it == registries.end()) {
		debugf("Warning: Puzzle %s not registered when looping.\n", id.data());
		return 0;
	}

	if (it->second.loop) {
		return it->second.loop(canvas, ms, post);
	}
	else {
		debugf("Warning: Puzzle %s has no loop function.\n", id.data());
		return 0;
	}
}

void PuzzleFactory::DestroyPuzzle(PuzzleMod* puzzleMod) const {
	if (!puzzleMod) {
		debugf("Warning: Puzzle is null when deleting.\n");
		return;
	}

	auto it = registries.find(puzzleMod->GetType());
	if (it == registries.end()) {
		debugf("Warning: Puzzle %s not registered when deleting.\n", puzzleMod->GetType());
		return;
	}

	if (it->second.deleter) {
		it->second.deleter(puzzleMod);
	}
	else {
		THROW_EXCEPTION(NullPointerException, "Puzzle " + string(puzzleMod->GetType()) + " deleter is null.\n");
	}
}

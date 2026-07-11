#include "asset.h"


using namespace std;

Asset::Asset(AssetFactory* factory, const string& asset) :
	mod(factory->CreateAsset(asset)),
	factory(factory),
	type(),
	name(),
	asset(),
	mobility(ASSET_ESTATE),
	weight(-1.f),
	size(-1.f),
	volume(-1.f),
	space(0.f),
	contents(),
	backpack(false) {
	if (!mod)
		THROW_EXCEPTION(NullPointerException, "Asset " + asset + " mod is null.\n");

	type = mod->GetType();
	name = mod->GetName();
}

Asset::~Asset() {
	factory->DestroyAsset(mod);
}

string Asset::GetType() const {
	return type;
}

string Asset::GetName() const {
	return name;
}

void Asset::DefineAsset() {
	mod->DefineAsset();
	asset = mod->asset;
	mobility = mod->mobility;
	weight = mod->weight;
	size = mod->size;
	volume = mod->volume;

	if (volume > 0.f) {
		space = volume;
	}
}

string Asset::GetAsset() {
	return asset;
}

void Asset::SetAsset(const string& asset) {
	this->asset = asset;
}

ASSET_MOBILITY Asset::GetMobility() const {
	return mobility;
}

float Asset::GetWeight() const {
	return weight;
}

float Asset::GetSize() const {
	return size;
}

float Asset::GetVolume() const {
	return volume;
}

bool Asset::GetBackpack() const {
	return backpack;
}

bool Asset::AddContent(const string& name, Asset* content) {
	if (content->size > space) return false;
	space -= content->size;
	weight += content->weight;
	contents[name] = content;
	return true;
}

void Asset::RemoveContent(const string& name) {
	auto it = contents.find(name);
	if (it == contents.end()) return;
	space += it->second->size;
	weight -= it->second->weight;
	contents.erase(it);
}

float Asset::GetSpace() const {
	return space;
}

const unordered_map<string, Asset*>& Asset::GetContents() const {
	return contents;
}

int EmptyAsset::count = 0;

EmptyAsset::EmptyAsset() : id(count++) {

}

EmptyAsset::~EmptyAsset() {

}

const char* EmptyAsset::GetId() {
	return "empty";
}

const char* EmptyAsset::GetType() const {
	return "empty";
}

const char* EmptyAsset::GetName() {
	name = "空资产" + to_string(id);
	return name.data();
}

void EmptyAsset::DefineAsset() {

}


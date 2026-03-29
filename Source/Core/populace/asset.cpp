#include "asset.h"


using namespace std;

Asset::Asset(AssetFactory* factory, const string& asset) :
	mod(factory->CreateAsset(asset)),
	factory(factory),
	type(mod->GetType()),
	name(mod->GetName()) {

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


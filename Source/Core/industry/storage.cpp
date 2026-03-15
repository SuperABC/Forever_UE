#include "storage.h"


using namespace std;

// 默认仓库

string DefaultStorage::GetId() {
	return "default";
}

string DefaultStorage::GetType() const {
	return "default";
}

string DefaultStorage::GetName() const {
	return "默认仓库";
}

vector<string> DefaultStorage::GetCategories() const {
	return { "默认分类" };
}

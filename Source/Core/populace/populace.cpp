#include "populace.h"
#include "utility.h"
#include "error.h"
#include "json.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <random>


using namespace std;

Populace::Populace() {

}

Populace::~Populace() {

}

void Populace::SetResourcePath(string path) {
	resourcePath = path;
}

void Populace::ReadConfigs(string path) const {
	path = resourcePath + path;
    if (!filesystem::exists(path)) {
        THROW_EXCEPTION(IOException, "Path does not exist: " + path + ".\n");
    }

    JsonReader reader;
    JsonValue root;

    ifstream fin(path);
    if (!fin.is_open()) {
        THROW_EXCEPTION(IOException, "Failed to open file: " + path + ".\n");
    }
    if (reader.Parse(fin, root)) {

    }
    else {
        fin.close();
        THROW_EXCEPTION(JsonFormatException, "Json syntax error: " + reader.GetErrorMessages() + ".\n");
    }
    fin.close();
}

void Populace::Init(int accomodation, vector<string> nameholders) {

}

void Populace::Destroy() {

}

void Populace::Tick(int day, int hour, int min, int sec) {

}

void Populace::Print() const {

}

void Populace::Load(string path) {

}

void Populace::Save(string path) const {

}

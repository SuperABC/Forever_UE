#include "society.h"
#include "utility.h"
#include "error.h"
#include "json.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <random>


using namespace std;

Society::Society() {

}

Society::~Society() {

}

void Society::SetResourcePath(string path) {
    resourcePath = path;
}

void Society::ReadConfigs(string path) const {
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

void Society::Init() {

}

void Society::Destroy() {

}

void Society::Tick(int day, int hour, int min, int sec) {

}

void Society::Print() const {

}

void Society::Load(string path) {

}

void Society::Save(string path) const {

}




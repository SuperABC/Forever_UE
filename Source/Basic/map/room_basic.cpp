#include "room_basic.h"


using namespace std;

int ResidentialRoom::count = 0;

ResidentialRoom::ResidentialRoom() : id(count++) {

}

ResidentialRoom::~ResidentialRoom() {

}

const char* ResidentialRoom::GetId() {
	return "residential";
}

const char* ResidentialRoom::GetType() const {
	return "residential";
}

const char* ResidentialRoom::GetName() {
	name = "住宅房间" + to_string(id);
	return name.data();
}

void ResidentialRoom::ConfigRoom(Quad* quad, FACE_DIRECTION direction) {
	isResidential = true;
	residentialCapacity = 1;

	//AddFurniture("/Game/Asset/Meshes/Cube.Cube", quad, quad->GetSizeX() / 2.f, 0.f, 0.f, 0.1f, 0.1f, 0.1f, 0.f, 0.f, 0.f, direction);
}

void ResidentialRoom::PlacePivots(Quad* room) {

}

int ShopRoom::count = 0;

ShopRoom::ShopRoom() : id(count++) {

}

ShopRoom::~ShopRoom() {

}

const char* ShopRoom::GetId() {
	return "shop";
}

const char* ShopRoom::GetType() const {
	return "shop";
}

const char* ShopRoom::GetName() {
	name = "商店房间" + to_string(id);
	return name.data();
}

void ShopRoom::ConfigRoom(Quad* quad, FACE_DIRECTION direction) {
	isWorkspace = true;
	workspaceCapacity = 100;
}

void ShopRoom::PlacePivots(Quad* room) {

}

int WarehouseRoom::count = 0;

WarehouseRoom::WarehouseRoom() : id(count++) {

}

WarehouseRoom::~WarehouseRoom() {

}

const char* WarehouseRoom::GetId() {
	return "warehouse";
}

const char* WarehouseRoom::GetType() const {
	return "warehouse";
}

const char* WarehouseRoom::GetName() {
	name = "仓库房间" + to_string(id);
	return name.data();
}

void WarehouseRoom::ConfigRoom(Quad* quad, FACE_DIRECTION direction) {
	isStorage = true;
	storageConfig = { {"shop", 100.f} };
}

void WarehouseRoom::PlacePivots(Quad* room) {

}

int ParkingRoom::count = 0;

ParkingRoom::ParkingRoom() : id(count++) {

}

ParkingRoom::~ParkingRoom() {

}

const char* ParkingRoom::GetId() {
	return "parking";
}

const char* ParkingRoom::GetType() const {
	return "parking";
}

const char* ParkingRoom::GetName() {
	name = "停车场房间" + to_string(id);
	return name.data();
}

void ParkingRoom::ConfigRoom(Quad* quad, FACE_DIRECTION direction) {
	isParking = true;
	parkingSpaces = { { 0, 0, 0, 0, 1, 0, 1, 0 } };
}

void ParkingRoom::PlacePivots(Quad* room) {

}

int FactoryRoom::count = 0;

FactoryRoom::FactoryRoom() : id(count++) {

}

FactoryRoom::~FactoryRoom() {

}

const char* FactoryRoom::GetId() {
	return "factory";
}

const char* FactoryRoom::GetType() const {
	return "factory";
}

const char* FactoryRoom::GetName() {
	name = "工厂房间" + to_string(id);
	return name.data();
}

void FactoryRoom::ConfigRoom(Quad* quad, FACE_DIRECTION direction) {
	isManufacture = true;
	manufactureTypes = { "experience" };
}

void FactoryRoom::PlacePivots(Quad* room) {

}

int LobbyRoom::count = 0;

LobbyRoom::LobbyRoom() : id(count++) {

}

LobbyRoom::~LobbyRoom() {

}

const char* LobbyRoom::GetId() {
	return "lobby";
}

const char* LobbyRoom::GetType() const {
	return "lobby";
}

const char* LobbyRoom::GetName() {
	name = "大堂" + to_string(id);
	return name.data();
}

void LobbyRoom::ConfigRoom(Quad* quad, FACE_DIRECTION direction) {
	isWorkspace = true;
	workspaceCapacity = 10;
}

void LobbyRoom::PlacePivots(Quad* room) {

}

int BunkRoom::count = 0;

BunkRoom::BunkRoom() : id(count++) {

}

BunkRoom::~BunkRoom() {

}

const char* BunkRoom::GetId() {
	return "bunk";
}

const char* BunkRoom::GetType() const {
	return "bunk";
}

const char* BunkRoom::GetName() {
	name = "客房" + to_string(id);
	return name.data();
}

void BunkRoom::ConfigRoom(Quad* quad, FACE_DIRECTION direction) {

}

void BunkRoom::PlacePivots(Quad* room) {

}

int OfficeRoom::count = 0;

OfficeRoom::OfficeRoom() : id(count++) {

}

OfficeRoom::~OfficeRoom() {

}

const char* OfficeRoom::GetId() {
	return "office";
}

const char* OfficeRoom::GetType() const {
	return "office";
}

const char* OfficeRoom::GetName() {
	name = "办公室" + to_string(id);
	return name.data();
}

void OfficeRoom::ConfigRoom(Quad* quad, FACE_DIRECTION direction) {
	isWorkspace = true;
	workspaceCapacity = 2;
}

void OfficeRoom::PlacePivots(Quad* room) {

}

int RegisterRoom::count = 0;

RegisterRoom::RegisterRoom() : id(count++) {

}

RegisterRoom::~RegisterRoom() {

}

const char* RegisterRoom::GetId() {
	return "register";
}

const char* RegisterRoom::GetType() const {
	return "register";
}

const char* RegisterRoom::GetName() {
	name = "挂号处" + to_string(id);
	return name.data();
}

void RegisterRoom::ConfigRoom(Quad* quad, FACE_DIRECTION direction) {
	isWorkspace = true;
	workspaceCapacity = 1;
}

void RegisterRoom::PlacePivots(Quad* room) {

}

int WaitingRoom::count = 0;

WaitingRoom::WaitingRoom() : id(count++) {

}

WaitingRoom::~WaitingRoom() {

}

const char* WaitingRoom::GetId() {
	return "waiting";
}

const char* WaitingRoom::GetType() const {
	return "waiting";
}

const char* WaitingRoom::GetName() {
	name = "等候室" + to_string(id);
	return name.data();
}

void WaitingRoom::ConfigRoom(Quad* quad, FACE_DIRECTION direction) {

}

void WaitingRoom::PlacePivots(Quad* room) {

}

int DoctorRoom::count = 0;

DoctorRoom::DoctorRoom() : id(count++) {

}

DoctorRoom::~DoctorRoom() {

}

const char* DoctorRoom::GetId() {
	return "doctor";
}

const char* DoctorRoom::GetType() const {
	return "doctor";
}

const char* DoctorRoom::GetName() {
	name = "诊室" + to_string(id);
	return name.data();
}

void DoctorRoom::ConfigRoom(Quad* quad, FACE_DIRECTION direction) {
	isWorkspace = true;
	workspaceCapacity = 1;
}

void DoctorRoom::PlacePivots(Quad* room) {

}

int MorgueRoom::count = 0;

MorgueRoom::MorgueRoom() : id(count++) {

}

MorgueRoom::~MorgueRoom() {

}

const char* MorgueRoom::GetId() {
	return "morgue";
}

const char* MorgueRoom::GetType() const {
	return "morgue";
}

const char* MorgueRoom::GetName() {
	name = "太平间" + to_string(id);
	return name.data();
}

void MorgueRoom::ConfigRoom(Quad* quad, FACE_DIRECTION direction) {

}

void MorgueRoom::PlacePivots(Quad* room) {

}

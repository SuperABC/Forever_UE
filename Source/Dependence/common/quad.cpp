#include "quad.h"

#include <algorithm>


using namespace std;

Quad::Quad() : posX(0.f), posY(0.f), sizeX(0.f), sizeY(0.f), acreage(0.f) {

}

Quad::Quad(float x, float y, float w, float h) : posX(x), posY(y), sizeX(w), sizeY(h), acreage(w * h * 100.f) {
    
}

float Quad::GetPosX() const {
    return posX;
}

void Quad::SetPosX(float x) {
    posX = x;
}

float Quad::GetPosY() const {
    return posY;
}

void Quad::SetPosY(float y) {
    posY = y;
}

float Quad::GetSizeX() const {
    return sizeX;
}

void Quad::SetSizeX(float w) {
    sizeX = w;
}

float Quad::GetSizeY() const {
    return sizeY;
}

void Quad::SetSizeY(float h) {
    sizeY = h;
}

float Quad::GetLeft() const {
    return posX - sizeX / 2.f;
}

float Quad::GetRight() const {
    return posX + sizeX / 2.f;
}

float Quad::GetBottom() const {
    return posY - sizeY / 2.f;
}

float Quad::GetTop() const {
    return posY + sizeY / 2.f;
}

void Quad::SetVertices(float x1, float y1, float x2, float y2) {
    if (x1 > x2) {
        swap(x1, x2);
    }
    if (y1 > y2) {
        swap(y1, y2);
    }

    posX = (x1 + x2) / 2.f;
    posY = (y1 + y2) / 2.f;
    sizeX = x2 - x1;
    sizeY = y2 - y1;
	acreage = sizeX * sizeY * 100.f;
}

void Quad::SetPosition(float x, float y, float w, float h) {
    posX = x;
    posY = y;
    sizeX = w;
    sizeY = h;
    acreage = sizeX * sizeY * 100.f;
}

float Quad::GetAcreage() const {
    return acreage;
}

void Quad::SetAcreage(float a) {
    acreage = a;
}

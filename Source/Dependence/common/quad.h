#pragma once


class Quad {
public:
	Quad();
	Quad(float x, float y, float w, float h);
	virtual ~Quad() = default;

	// 中心点坐标
	float GetPosX() const;
	void SetPosX(float x);
	float GetPosY() const;
	void SetPosY(float y);

	// 长宽尺寸
	float GetSizeX() const;
	void SetSizeX(float w);
	float GetSizeY() const;
	void SetSizeY(float h);

	// 四边坐标
	float GetLeft() const;
	float GetRight() const;
	float GetBottom() const;
	float GetTop() const;

	// 通过四边坐标指定位置
	void SetVertices(float x1, float y1, float x2, float y2);

	// 通过中心和尺寸指定位置
	void SetPosition(float x, float y, float w, float h);

	// 面积
	float GetAcreage() const;
	void SetAcreage(float a);

protected:
	float posX, posY;
	float sizeX, sizeY;

	float acreage;
};

#pragma once

#include "class.h"


/*
* 计算点到线段的最近点参数t（[0,1]，已裁剪）
* @px, py: 待投影的点
* @ax, ay, bx, by: 线段两端点
*/
float ProjectOntoLine(float px, float py, float ax, float ay, float bx, float by);

/*
* 按精细程度从房间到地块依次取第一个可用的导航节点（房间取自身中心点，建筑/园区/地块取四角中第一个有效角点）
* @room, building, zone, block: 候选位置，按精细程度从高到低传入
*/
Node* ResolveNavigationNode(Room* room, Building* building, Zone* zone, Block* block);

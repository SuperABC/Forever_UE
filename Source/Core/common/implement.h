#pragma once

#include "../../Dependence/common/handle.h"

class Map;
class Populace;
class Society;
class Story;
class Industry;
class Traffic;
class Player;

class PostImplement : public PostHandle {
public:

	/*
	* 构造查询实现
	* @map, populace, society, story, industry, traffic, player: 七大子系统指针
	*/
	PostImplement(Map* map, Populace* populace, Society* society,
		Story* story, Industry* industry, Traffic* traffic, Player* player);

	/*
	* 析构查询实现
	*/
	virtual ~PostImplement();

	/*
	* 向Core发起查询请求，根据请求类型分发给对应子系统
	* @request: 查询请求JSON，包含type字段指定查询类型
	* @return: 查询结果JSON
	*/
	JsonValue* Post(JsonValue* request) override;

private:

	// Map子系统
	Map* map;

	// Populace子系统
	Populace* populace;

	// Society子系统
	Society* society;

	// Story子系统
	Story* story;

	// Industry子系统
	Industry* industry;

	// Traffic子系统
	Traffic* traffic;

	// Player子系统
	Player* player;

};

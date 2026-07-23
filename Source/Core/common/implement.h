#pragma once

#include "../../Dependence/common/json.h"
#include "../../Dependence/common/handle.h"

#include "map/map.h"
#include "map/room.h"
#include "populace/populace.h"
#include "society/society.h"
#include "story/story.h"
#include "industry/industry.h"
#include "traffic/traffic.h"
#include "player/player.h"

#include <string>
#include <vector>


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
	* 向Core发起查询请求，根据请求类型分发给对应子系统，结果写入result
	* @request: 查询请求JSON，包含type字段指定查询类型
	*/
	void Post(const JsonValue& request) override;

	/*
	* 获取上一次Post()查询的结果
	* @return: 查询结果JSON的引用
	*/
	const JsonValue& GetResult() const override;

private:

	// 查询结果，Post()每次调用时整体重新赋值以清空上一次的残留内容
	JsonValue result;

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

	/*
	* 按照类型随机选取一个房间
	*/
	Room* RandomRoom(std::string type);
};

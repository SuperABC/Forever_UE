#pragma once

class JsonValue;

// 向Core发起查询的句柄基类，定义在Dependence层供所有mod使用
class PostHandle {
public:

	/*
	* 析构查询句柄
	*/
	virtual ~PostHandle() = default;

	/*
	* 向Core发起查询请求
	* @request: 查询请求JSON
	* @return: 查询结果JSON
	*/
	virtual JsonValue* Post(JsonValue* request) = 0;

};

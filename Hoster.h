#pragma once

#include <memory>
#include <functional>

#include <behaviac.h>

#include "Entity.h"
#include "Asset.h"
#include "MessageDispatcher.h"

namespace Adoter
{
namespace pb = google::protobuf;

/*
 * 居民
 *
 * */
class Hoster : public Entity, public behaviac::Agent
{
private:
	Asset::Hoster _stuff;
	int32_t _begin_time;
public:
	DECLARE_BEHAVIAC_AGENT(Hoster, Agent);
public:
	~Hoster() { behaviac::Agent::Destroy(this); }
	Hoster() { }
	Hoster(int64_t hoster_id);
	Hoster(Asset::Hoster& hoster);
	Hoster(Asset::Hoster* hoster);

	virtual bool HandleMessage(const Asset::MsgItem& item) override;

	bool Update(int32_t heart_count);
	//所有CLASS_NAME + Asset::CLASS_NAME组合的结构设计，建议都定义该函数
	const Asset::Hoster& Get() { return this->_stuff; } 
	const Asset::CommonProp& GetHumanProp() { return this->_stuff.human_prop(); } //获取基础属性，比如级别、性别...
	
	void SetCommonProp(Asset::Hoster* hoster);
public:
	////行为树相关
	bool Condition();
	behaviac::EBTStatus Action1();
	behaviac::EBTStatus Action2();

	void Run() { std::cout << "I am running." << std::endl; } //跑
	void Gather() { std::cout << "I am gathering." << std::endl; } //采集
	void Farm() { std::cout << "I am farming." << std::endl; } //种植
	void Mine() { std::cout << "I am mining." << std::endl; } //采矿
	void Wander() { 
		int32_t x = CommonUtil::Random(-1, 1);	
		int32_t z = CommonUtil::Random(-1, 1);	
		Asset::Vector3 position = this->_stuff.human_prop().position();
		position.set_x(position.x() + x);
		position.set_z(position.z() + z);
		std::cout << "I am wandering:" << this->_ID << " POS :" << position.x() << " " << position.z() << std::endl;
	} 
private:
	////行为树相关
	int m_iX;
	int m_iY;
	unsigned int m_iBaseSpeed;
	int m_Frames;
};

/*
 * 居民管理
 *
 * */
class HosterManager
{
private:
	std::unordered_set<std::shared_ptr<Hoster>> _hosters;
public:
	static HosterManager& Instance()
	{
		static HosterManager _instance;
		return _instance;
	}
	
	//加载HOSTER数据
	bool Load();
	//定时刷新HOSTER
	bool Update(int32_t diff);
};

#define HosterInstance HosterManager::Instance()

}

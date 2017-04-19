#include "Hoster.h"
#include "Timer.h"
#include "Protocol.h"
#include "CommonUtil.h"
#include "RedisManager.h"

#include <iostream>
#include <hiredis.h>

namespace Adoter
{

/*
 * 居民
 * 
 * */

Hoster::Hoster(Asset::Hoster& hoster)
{
	_stuff = hoster;	//居民属性
	_super_common_prop = hoster.human_prop(); //基类公共属性
	
	_begin_time = GetTime();
}

Hoster::Hoster(Asset::Hoster* hoster)
{
	_stuff = *hoster; //居民属性
	_super_common_prop = hoster->human_prop(); //基类公共属性
	
	_begin_time = GetTime();
}

Hoster::Hoster(int64_t hoster_id) 
{
	pb::Message* message = AssetInstance.Get(hoster_id); //数据初始化 
	if (message)
	{
		Asset::Hoster* hoster = dynamic_cast<Asset::Hoster*>(message);
		if (hoster) 
		{
			_stuff = *hoster; //居民属性
			_super_common_prop = hoster->human_prop(); //基类公共属性
		}
	}
	
	_begin_time = GetTime();
}

void Hoster::SetCommonProp(Asset::Hoster* hoster)
{
	_stuff.CopyFrom(*hoster);
	_super_common_prop.CopyFrom(hoster->human_prop());

	_begin_time = GetTime();
}

bool Hoster::HandleMessage(const Asset::MsgItem& item)
{
	switch (item.type())
	{
		//进入视野
		case Asset::MSG_TYPE_AOI_ENTER:
		{
			if (item.sender() == item.receiver()) return false; //这种协议不处理
			Asset::CommonProp common_prop;
			//if (!common_prop.ParseFromString(item.content())) return;
			//if (!IsInSight(common_prop.position())) return;	//如果看不到该实体，则退出
		}
		break;
		//离开视野
		case Asset::MSG_TYPE_AOI_LEAVE:
		{
			if (item.sender() == item.receiver()) return false; //这种协议不处理
			Asset::CommonProp common_prop;
		}
		break;
		//移动	
		case Asset::MSG_TYPE_AOI_MOVE:
		{
			if (item.sender() == item.receiver()) return false; //这种协议不处理
			Asset::CommonProp common_prop;
			//if (!common_prop.ParseFromString(item.content())) return;
		}
		break;
		
		default:
		{
			std::cout << __func__ << ":Erroooooooooooooooooooooooooor:No handler, type:" << item.type() << std::endl;
		}
		break;
	}
	
	//std::cout << __func__ << " of Hoster call:sender:" << item.sender() << ", receiver:" << item.receiver() << ", type:" << item.type() << std::endl;
	return true;
}
	
bool Hoster::Update(int32_t heart_count) //50MS
{
	auto curr_scene = GetScene();
	if (!curr_scene) return false;

	/*
	if (heart_count % 20 == 0) //1S
	{
		int32_t speed = this->_stuff.human_prop().speed();
		Asset::Vector3 pos;
		int32_t x = CommonUtil::Random(-speed, speed);	
		int32_t z = CommonUtil::Random(-speed, speed);	
		//随机移动
		Asset::Vector3 position = this->_stuff.human_prop().position();
		position.set_x(position.x() + x);
		position.set_z(position.z() + z);

		//std::cout << "---------ID:" << this->_ID << " POS :" << position.x() << " " << position.z() << std::endl;

		this->StepMove(position);
	}
	*/

	behaviac::Workspace::GetInstance()->SetTimeSinceStartup(GetTime() - _begin_time);

	behaviac::EBTStatus status = btexec();
	//std::cout << "Behavi tree running, status:" << status << std::endl;
	if (status == behaviac::BT_RUNNING)
	{
		std::cout << "Behaviac tree running, heart_count:" << heart_count << " ID:" << this->_ID << std::endl;
	}

	return true;
}

bool Hoster::Condition()
{
	return true;
}

behaviac::EBTStatus Hoster::Action1()
{
	return behaviac::BT_RUNNING;
}

behaviac::EBTStatus Hoster::Action2()
{
	return behaviac::BT_RUNNING;
}

//////////////////////////////////////////////////
// 行为树
//////////////////////////////////////////////////

BEGIN_PROPERTIES_DESCRIPTION(Hoster)
{
	REGISTER_PROPERTY(m_iBaseSpeed);

	REGISTER_METHOD(Condition);
	REGISTER_METHOD(Action1);
	REGISTER_METHOD(Action2);
	
	REGISTER_METHOD(Run);
	REGISTER_METHOD(Gather);
	REGISTER_METHOD(Farm);
	REGISTER_METHOD(Mine);
	REGISTER_METHOD(Wander);
}
END_PROPERTIES_DESCRIPTION()

/*
 * 居民管理
 * 
 * */

bool HosterManager::Load()
{
	auto& hosters = AssetInstance.GetMessagesByType("Asset::ASSET_TYPE_HOSTER");
	for (auto m : hosters)
	{
		Asset::Hoster* h = dynamic_cast<Asset::Hoster*>(m);
		if (!h) return false;

		int64_t scene_id = h->human_prop().scene_id();
		auto scene = SceneInstance.Get(scene_id);
		if (!scene)
		{
			std::cerr << "Hoster:" << h->common_prop().global_id() << " whoes scene id is "<< scene_id << " is invalid." << std::endl;
			continue;
		}
		
		std::shared_ptr<Hoster> hoster(behaviac::Agent::Create<Hoster>()); 
		hoster->SetCommonProp(h); //基本配置数据

		if (!h->behaviac_tree_path().empty()) //是否具有行为树数据
		{
			const char* tree_name = h->behaviac_tree_path().c_str();
			bool ret = hoster->btload(tree_name);
			if (ret) hoster->btsetcurrent(tree_name);
		}

		hoster->SetID(h->common_prop().global_id()); //比较特殊，具备玩家和资源的双重属性，其全局ID采用资源ID，不重新分配.
		hoster->OnBirth(scene_id); //居民出生

		_hosters.emplace(hoster);
	}
	return true;
}

bool HosterManager::Update(int32_t diff)
{
	for (auto hoster : _hosters)
	{
		hoster->Update(diff);
	}
	return true;
}

}

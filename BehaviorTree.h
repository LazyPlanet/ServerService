#pragma once

#include <behaviac/behaviac.h>
#include "Hoster.h"

namespace Adoter
{

/*
 * 行为树，使用腾讯开源项目 behaviac 进行的扩展，自动绑定ASSET中的HOSTER...
 *
 * 从而实现强大的AI系统.
 *
 * */
class BTManager
{
private:
	Hoster* g_player;
public:
	BTManager();

	static BTManager& Instance()
	{
		static BTManager _instance;
		return _instance;
	}
public:
	bool InitBehaviac(behaviac::Workspace::EFileFormat ff);

	bool InitHoster(const char* tree_name);

	void UpdateLoop();

	void CleaupHoster();

	void CleanupBehavic();

};

#define BehaviorTreeInstance BTManager::Instance()

}

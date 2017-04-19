#include "BehaviorTree.h"

namespace Adoter
{

BTManager::BTManager()
{

}

bool BTManager::InitBehaviac(behaviac::Workspace::EFileFormat ff)
{
	behaviac::Config::SetSocketBlocking(false);
	behaviac::Config::SetSocketPort(40002);

	behaviac::Agent::Register<Hoster>();

	behaviac::Workspace::GetInstance()->SetFilePath("./Asset/Behaviac/exported");
	behaviac::Workspace::GetInstance()->SetFileFormat(ff);

	behaviac::Workspace::GetInstance()->ExportMetas("./Asset/Behaviac/meta/metadata.xml"); //导出元数据

	return true;
}

bool BTManager::InitHoster(const char* tree_name)
{
	auto g_hoster = behaviac::Agent::Create<Hoster>();

	bool ret = g_player->btload(tree_name);
	if (!ret) return false;

	g_player->btsetcurrent(tree_name);

	return true;
}

void BTManager::UpdateLoop()
{
	int frames = 0;
	behaviac::EBTStatus status = behaviac::BT_RUNNING;

	while (status == behaviac::BT_RUNNING)
	{
		std::cout << "frame " << ++frames << std::endl;
		//status = g_player->btexec();
	}
}

void BTManager::CleaupHoster()
{
	behaviac::Agent::Destroy(g_player);
}

void BTManager::CleanupBehavic()
{
	behaviac::Agent::UnRegister<Hoster>();
	behaviac::Workspace::GetInstance()->Cleanup();
}

}

#include <iostream>
#include "WorldNodeServer.h"
#include "ServerComm/Config.h"
#include "easylogging/easylogging++.h"

bool WorldNodeServer::InitHelper()
{
	m_pNetModule->NFINetModule::AddReceiveCallBack(this, &WorldNodeServer::OtherMessage);


	//init server info
	int ret = m_pNetModule->Initialization(
		g_pConfig->m_ServerConf["NodeIp"].asCString(),
		g_pConfig->m_ServerConf["NodePort"].asInt(),
		g_pConfig->m_ServerConf["MaxConnect"].asInt()
	);
	if (ret < 0)
	{
		LOG(ERROR) << "init WorldNodeServer failed";
		return false; //log
	}
	return true;
}

void WorldNodeServer::OtherMessage(const NFSOCK nSockIndex, const int nMsgID, const char* msg,const uint32_t nLen)
{

}


#include <iostream>
#include "DBNodeServer.h"
#include "ServerComm/Config.h"
#include "easylogging/easylogging++.h"

bool DBNodeServer::InitHelper()
{
	m_pNetModule->NFINetModule::AddReceiveCallBack(this, &DBNodeServer::OtherMessage);


	//init server info
	int ret = m_pNetModule->Initialization(
		g_pConfig->m_ServerConf["NodeIp"].asCString(),
		g_pConfig->m_ServerConf["NodePort"].asInt(),
		g_pConfig->m_ServerConf["MaxConnect"].asInt()
	);
	if (ret < 0)
	{
		LOG(ERROR) << "init DBNodeServer failed";
		return false; //log
	}
	return true;
}

void DBNodeServer::OtherMessage(const NFSOCK nSockIndex, const int nMsgID, const char* msg,const uint32_t nLen)
{

}


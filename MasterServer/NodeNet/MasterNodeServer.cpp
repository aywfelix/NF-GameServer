#include "MasterNodeServer.h"
#include "ServerProto/NFNodeNet.pb.h"
#include "easylogging/easylogging++.h"
#include "ServerComm/Config.h"

using namespace NFNetProto;

bool MasterNodeServer::InitHelper()
{
	//init server info
	int ret = m_pNetModule->Initialization(
		g_pConfig->m_ServerConf["NodeIp"].asCString(),
		g_pConfig->m_ServerConf["NodePort"].asInt(),
		g_pConfig->m_ServerConf["MaxConnect"].asInt()
	);
	if (ret < 0)
	{
		LOG(ERROR) << "init MasterNodeServer failed";
		return false;
	}
	return true;
}

void MasterNodeServer::AfterReportToServer(NF_SHARE_PTR<ServerData> pReportServerData)
{
	EServerType nExcludeType = (EServerType)pReportServerData->pData->server_type();
    SyncNodeDataToAll(nExcludeType);
	LOG(INFO) << "sync node server info: name" << pReportServerData->pData->server_name();
}

void MasterNodeServer::SyncNodeDataToAll(EServerType nType)
{
    ServerInfoReportList report_list;
    NF_SHARE_PTR<ServerData> pServerData = m_mClientNodes.First();
    while (pServerData)
    {
        ServerInfoReport* pData = report_list.add_server_list();
        *pData = *(pServerData->pData);
		pServerData = m_mClientNodes.Next();
    }

    pServerData = m_mClientNodes.First();
    while (pServerData)
    {
        m_pNetModule->SendMsgPB(MASTER_REPORT_SERVER_INFO_TO_SERVER, report_list, pServerData->nFD);
        pServerData = m_mClientNodes.Next();
    }
}

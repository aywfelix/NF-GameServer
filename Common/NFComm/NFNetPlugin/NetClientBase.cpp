#include "NetClientBase.h"
#include "ServerProto/NFNodeNet.pb.h"
#include "easylogging/easylogging++.h"
#include "ServerComm/Config.h"

using namespace NFNetProto;

bool NetClientBase::Init()
{
	m_pNetClientModule = new NFNetClientModule();
    m_pNetClientModule->ExpandBufferSize(10*1024*1024);
    m_pNetClientModule->NFINetClientModule::AddReceiveCallBack(EServerType::SERVER_TYPE_MASTER, MASTER_REPORT_SERVER_INFO_TO_SERVER, this, &NetClientBase::OnMasterMessage);
	return true;
}



NF_SHARE_PTR<ConnectData> NetClientBase::GetServerNetInfo(const int nServerID)
{
	return m_pNetClientModule->GetServerNetInfo(nServerID);
}

NF_SHARE_PTR<ConnectData> NetClientBase::GetServerNetInfo(const NFSOCK nSockIndex)
{
	return m_pNetClientModule->GetServerNetInfo(nSockIndex);
}

void NetClientBase::OnSocketNodeEvent(const NFSOCK nSockIndex, const NF_NET_EVENT nEvent, NFINet* pNet)
{
	NF_SHARE_PTR<ConnectData> pServerData = m_pNetClientModule->GetServerNetInfo(pNet);
    if(nEvent == NF_NET_EVENT_CONNECTED)
    {
        if(pServerData)
        {
            int nServerId = pServerData->nGameID;
            m_pNetClientModule->SendToServerByPB(nServerId, REPORT_CLIENT_INFO_TO_SERVER, m_serverInfoReport);
			LOG(INFO) << m_serverInfoReport.server_name() << " connect to " << pServerData->strName << " ok";
        }
    }
	else
	{
		LOG(ERROR) << "connect close!!!";
		if (pServerData)
		{
			LOG(ERROR)  << "close connect between(" << m_serverInfoReport.server_name()<<","<<pServerData->strName<<")";
		}
	}
    
}

void NetClientBase::OnMasterMessage(const NFSOCK nSockIndex, const int nMsgID, const char *msg, const uint32_t nLen)
{

    NFGUID nGuid;
    ServerInfoReportList report_list;
    if (!NFINetModule::ReceivePB(nMsgID, msg, nLen, report_list, nGuid))
    {
        return;
    }
    ConnectData xServerData;
    for (int i = 0; i < report_list.server_list_size(); ++i)
    {
        const ServerInfoReport& server_info = report_list.server_list(i);

        for (auto serverType : m_ConnectType)
        {
            if (server_info.server_type() == serverType)
            {
                xServerData.nGameID = server_info.server_id();
                xServerData.nPort = server_info.server_port();
                xServerData.strName = server_info.server_name();
                xServerData.strIP = server_info.server_ip();
                xServerData.eEServerType = EServerType(server_info.server_type());
                m_pNetClientModule->AddServer(xServerData); 
            }
        }
    }    
}


bool NetClientBase::Loop()
{
    return m_pNetClientModule->Execute();
}


void NetClientBase::AddConnectMaster()
{
	if (this->GetServerType() != EServerType::SERVER_TYPE_MASTER)
	{
		ConnectData xServerData;
		xServerData.nGameID = g_pConfig->m_Root["MasterServer"]["NodeID"].asInt();
		xServerData.eEServerType = EServerType::SERVER_TYPE_MASTER;
		xServerData.strIP = g_pConfig->m_Root["MasterServer"]["NodeIp"].asString();
		xServerData.nPort = g_pConfig->m_Root["MasterServer"]["NodePort"].asInt();
		xServerData.strName = g_pConfig->m_Root["MasterServer"]["NodeName"].asString();
		m_pNetClientModule->AddServer(xServerData);
	}
}

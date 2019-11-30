#include "NFNetPlugin/NetServerBase.h"
#include "ServerProto/NFNodeNet.pb.h"
#include "easylogging/easylogging++.h"

bool NetServerBase::Init()
{
    m_pNetModule = new NFNetModule();
    m_pNetModule->ExpandBufferSize(10*1024*1024);
    m_pNetModule->NFINetModule::AddEventCallBack(this, &NetServerBase::OnClientSocketEvent);
    m_pNetModule->NFINetModule::AddReceiveCallBack(REPORT_CLIENT_INFO_TO_SERVER, this, &NetServerBase::OnReportToServer);
	return true;
}


void NetServerBase::OnClientSocketEvent(const NFSOCK nSockIndex, const NF_NET_EVENT nEvent, NFINet *pNet)
{
	switch (nEvent)
	{
	case NF_NET_EVENT_CONNECTED:
		OnClientConnected(nSockIndex);
		break;
	case NF_NET_EVENT_EOF:
	case NF_NET_EVENT_ERROR:
	case NF_NET_EVENT_TIMEOUT:
	default:
		OnClientDisconnect(nSockIndex);
		break;
	}
}

void NetServerBase::OnClientConnected(const NFSOCK nSockIndex)
{
	LOG(INFO) <<" client sock(" << nSockIndex << ") connected";
}


void NetServerBase::OnClientDisconnect(const NFSOCK nSockIndex)
{
    NF_SHARE_PTR<ServerData> pServerData = m_mClientNodes.First();
    int nServerId = 0;
    while(pServerData)
    {
        if(pServerData->nFD == nSockIndex)
        {
			nServerId = pServerData->pData->server_id();
            break;
        }
        pServerData = m_mClientNodes.Next();
    }
    m_mClientNodes.RemoveElement(nServerId);
	LOG(INFO) << "client(" << nServerId << " " << pServerData->pData->server_name()
		<< " " << pServerData->pData->server_ip() << ") disconnected";
}


void NetServerBase::OnReportToServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg,const uint32_t nLen)
{
    NFGUID nPlayerID;
    NFNetProto::ServerInfoReport xMsg;
    if (!NFINetModule::ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    auto pServerData = NF_SHARE_PTR<ServerData>(NF_NEW ServerData());
    pServerData->nFD = nSockIndex;
    *(pServerData->pData) = xMsg;
    m_mClientNodes.RemoveElement(xMsg.server_id());
    m_mClientNodes.AddElement(xMsg.server_id(), pServerData);

    AfterReportToServer(pServerData);
	LOG(INFO) << " report server info from " << xMsg.server_id();
}

void NetServerBase::AfterReportToServer(NF_SHARE_PTR<ServerData> pReportServerData)
{

}

NF_SHARE_PTR<ServerData> NetServerBase::GetClientNodeData(int nServerId)
{
    NF_SHARE_PTR<ServerData> pServerData = m_mClientNodes.GetElement(nServerId);
	if (pServerData == nullptr)
	{
		return nullptr;
	}

    return pServerData;    
}


bool NetServerBase::Loop()
{
	return m_pNetModule->Execute();
}

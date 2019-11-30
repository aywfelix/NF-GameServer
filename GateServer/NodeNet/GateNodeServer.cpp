#include <iostream>
#include "GateNodeServer.h"
#include "ServerComm/Config.h"
#include "ClientModule/ModuleChat.h"
#include "ClientPlayer/ClientPlayerMgr.h"
#include "ClientPlayer/ClientPlayer.h"
#include "NodeNet/GateServerThread.h"
#include "easylogging/easylogging++.h"

bool GateNodeServer::InitHelper()
{
	m_pNetModule->NFINetModule::AddReceiveCallBack(this, &GateNodeServer::OtherMessage);
	m_pNetModule->NFINetModule::AddReceiveCallBack(LOGIN_ROUTE_TO_GATE, this, &GateNodeServer::OnLoginRouteBack);
	m_pNetModule->NFINetModule::AddReceiveCallBack(CHAT_ROUTE_TO_GATE, this, &GateNodeServer::OnChatRouteBack);

	//init server info
	int ret = m_pNetModule->Initialization(
		g_pConfig->m_ServerConf["NodeIp"].asCString(),
		g_pConfig->m_ServerConf["NodePort"].asInt(),
		g_pConfig->m_ServerConf["MaxConnect"].asInt()
	);
	if (ret < 0)
	{
		LOG(ERROR) << "init GateNodeServer failed";
		return false; //log
	}
	return true;
}

void GateNodeServer::OtherMessage(const NFSOCK nSockIndex, const int msgid, const char *msg, const uint32_t nLen)
{
	LOG(INFO) << "Gate Server recv other message msgid:" << msgid;
}

bool GateNodeServer::SendPackToLogin(const NFSOCK client_fd, const int msgid, const char *msg, uint32_t nLen, int nGameID, const std::string &ip)
{
	return true;
}
bool GateNodeServer::SendPackToScene(const int msgid, const google::protobuf::Message &xData, int nServerID)
{
	return true;
}
bool GateNodeServer::SentPackToChat(const int msgid, const google::protobuf::Message &xData)
{
	NF_SHARE_PTR<ServerData> pServerData = m_mClientNodes.First();
	std::vector<NF_SHARE_PTR<ServerData>> typed_list;
	while (pServerData)
	{
		if (EServerType::SERVER_TYPE_CHAT == pServerData->pData->server_type())
		{
			typed_list.push_back(pServerData);
		}
		pServerData = m_mClientNodes.Next();
	}
	if (typed_list.size() == 0)
	{
		LOG(ERROR) << "GateNodeServer::SentPackToChat Send ERROR!!!!";
		return false;
	}
	return m_pNetModule->SendMsgPB(msgid, xData, typed_list[0]->nFD);
}
bool GateNodeServer::SentPackToWorld(const int msgid, const google::protobuf::Message &xData)
{
	return true;
}

void GateNodeServer::OnLoginRouteBack(NFSOCK nSockIndex, const int msgid, const char *msg, const uint32_t nLen)
{
}

void GateNodeServer::OnChatRouteBack(NFSOCK nSockIndex, const int msgid, const char *msg, const uint32_t nLen)
{
	NFGUID nGUID;
	ChatToGatePacket xData;
	if (!m_pNetModule->ReceivePB(msgid, msg, nLen, xData, nGUID))
	{
		return;
	}

	uint64_t playerId = xData.player_id();

	if (ModuleChat::RPC_CHAT_CHAT_REQ == xData.msg_id())
	{
		ClientPlayer* pPlayer = g_pClientPlayerMgr->GetPlayerByID(playerId);
		if (pPlayer == nullptr)
		{
			return;
		}

		NFSOCK nPlayerSock = pPlayer->GetSockFd();
		if (nPlayerSock == -1)
		{
			return;
		}
		g_pServerThread->PlayerServer().SentToClient(xData.msg_id(), xData.msg_body(), nPlayerSock);
	}

	LOG(INFO) << "OnChatRouteBack and sent to client: socket_id:" << playerId << " " << xData.msg_id();

}
void GateNodeServer::OnWorldRouteBack(NFSOCK nSockIndex, const int msgid, const char *msg, const uint32_t nLen)
{
}
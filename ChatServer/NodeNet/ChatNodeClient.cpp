#include "ChatNodeClient.h"
#include "ServerComm/Config.h"
#include "Packet/PacketMgr.h"
#include "ServerComm/Util.h"
#include "Packet/Player.h"
#include "MsgHandle/ChatPlayer.h"


void ChatNodeClient::InitHelper()
{
    m_pNetClientModule->NFINetClientModule::AddEventCallBack(EServerType::SERVER_TYPE_MASTER, this, &ChatNodeClient::OnSocketEvent);
	m_pNetClientModule->NFINetClientModule::AddEventCallBack(EServerType::SERVER_TYPE_GATE, this, &ChatNodeClient::OnSocketEvent);
	m_pNetClientModule->NFINetClientModule::AddReceiveCallBack(EServerType::SERVER_TYPE_GATE, GATE_ROUTE_TO_CHAT, this, &ChatNodeClient::OnGateRouteChat);
	SetServerInfoReport();
	AddConnectServer();
}

void ChatNodeClient::SetServerInfoReport()
{
    m_serverInfoReport.set_server_id(g_pConfig->m_ServerConf["NodeId"].asInt());
    m_serverInfoReport.set_server_name(g_pConfig->m_ServerConf["NodeName"].asString());
    m_serverInfoReport.set_server_cur_count(0);
    m_serverInfoReport.set_server_ip(g_pConfig->m_ServerConf["NodeIp"].asString());
    m_serverInfoReport.set_server_port(g_pConfig->m_ServerConf["NodePort"].asInt());
    m_serverInfoReport.set_server_max_online(2000);
    m_serverInfoReport.set_server_state(EServerState::EST_NORMAL);
    m_serverInfoReport.set_server_type(SERVER_TYPE_CHAT);
}

//void ChatNodeClient::AddConnectMaster()
//{
//	ConnectData xServerData;
//	xServerData.nGameID = g_pConfig->m_Root["MasterServer"]["NodeID"].asInt();
//	xServerData.strIP = g_pConfig->m_Root["MasterServer"]["NodeIp"].asString();
//	xServerData.nPort = g_pConfig->m_Root["MasterServer"]["NodePort"].asInt();
//	xServerData.strName = g_pConfig->m_Root["MasterServer"]["NodeName"].asString();
//	xServerData.eEServerType = EServerType::SERVER_TYPE_MASTER;
//	m_pNetClientModule->AddServer(xServerData);
//}

void ChatNodeClient::AddConnectServer()
{
	AddConnectMaster();
	m_ConnectType.emplace_back(SERVER_TYPE_GATE);
}


void ChatNodeClient::OnSocketEvent(const NFSOCK nSockIndex, const NF_NET_EVENT nEvent, NFINet* pNet)
{
    OnSocketNodeEvent(nSockIndex, nEvent, pNet);
}

void ChatNodeClient::OnGateRouteChat(const NFSOCK nSockIndex, const int msgid, const char* msg, const uint32_t msglen)
{
	NFGUID playerId;
	GateToChatPacket xData;
	if (!NFINetModule::ReceivePB(msgid, msg, msglen, xData, playerId))
	{
		return;
	}
	NF_SHARE_PTR<ConnectData> pServerData = GetServerNetInfo(nSockIndex);
	if (!pServerData)
	{
		return; //TODO pServerData allways null
	}
	//parse the packet
	Packet* pRecvPacket = g_pPacketMgr->CreatePakcet(xData.msg_id(), xData.msg_body().c_str(), xData.msg_body().length());
	AutoDelete<Packet> delRecvPacket(pRecvPacket);

	MsgHandle msgHandle = g_pPacketMgr->GetMsgHandle(xData.msg_id());
	if (msgHandle == nullptr)
	{
		return;
	}
	ChatPlayer* pPlayer = new ChatPlayer;
	AutoDelete<Player> delPlayer(pPlayer);
	pPlayer->m_PlayerId = xData.player_id();
	pPlayer->m_GameId = pServerData->nGameID;
	int ret = msgHandle(pPlayer, pRecvPacket);  //process msg logic

}

void ChatNodeClient::SendToGate(const int GameID, uint64_t playerId, uint32_t msg_id, ::google::protobuf::Message* pMsg)
{
	std::string send_msg;

	ChatToGatePacket chattogate;
	pMsg->SerializeToString(&send_msg);
	chattogate.Clear();
	chattogate.set_msg_id(msg_id);
	chattogate.set_msg_body(send_msg);
	chattogate.set_player_id(playerId);
	m_pNetClientModule->SendToServerByPB(GameID, CHAT_ROUTE_TO_GATE, chattogate);
}
#include "Client.h"
#include "ServerComm/Config.h"
#include "ClientProto/Chat.pb.h"
#include "easylogging/easylogging++.h"
#include "ClientModule/ModuleChat.h"
#include "ServerComm/Util.h"

bool Client::Init()
{
	m_pNetClientModule = new NFNetClientModule();
	m_pNetClientModule->ExpandBufferSize(10 * 1024 * 1024);
	
	m_pNetClientModule->NFINetClientModule::AddEventCallBack(EServerType::SERVER_TYPE_GATE, this, &Client::OnSocketEvent);
	m_pNetClientModule->NFINetClientModule::AddReceiveCallBack(EServerType::SERVER_TYPE_GATE, this, &Client::OnMessage);
	
	ConnectServer();
	m_ServerId = g_pConfig->m_Root["GatePlayerServer"]["NodeId"].asInt();
	return true;
}

void Client::ConnectServer()
{
	//set gate server info
	ConnectData xServerData;
	xServerData.nGameID = g_pConfig->m_Root["GatePlayerServer"]["NodeId"].asInt();
	xServerData.nPort = g_pConfig->m_Root["GatePlayerServer"]["NodePort"].asInt();
	xServerData.strName = g_pConfig->m_Root["GatePlayerServer"]["NodeName"].asString();
	xServerData.strIP = g_pConfig->m_Root["GatePlayerServer"]["NodeIp"].asString();
	xServerData.eEServerType = EServerType::SERVER_TYPE_GATE;
	m_pNetClientModule->AddServer(xServerData);
}

bool Client::Loop()
{
	while (1)
	{
		m_pNetClientModule->Execute();
		MySleep(10);
	}
}

void Client::OnSocketEvent(const NFSOCK nSockIndex, const NF_NET_EVENT nEvent, NFINet* pNet)
{
	if (nEvent == NF_NET_EVENT_CONNECTED)
	{
		LOG(INFO) << "client connect to gate ok";
		//send msg to gate
		std::string send_msg;
		Chat_ChatReq chat_msg;
		chat_msg.set_channel(1);
		chat_msg.set_chat_msg("this is test msg to chat server!!!");

		chat_msg.SerializeToString(&send_msg);
		
		m_pNetClientModule->SendByServerIDWithOutHead(m_ServerId, (const uint16_t)(ModuleChat::RPC_CHAT_CHAT_REQ), send_msg);
	}
}

void Client::OnMessage(const NFSOCK nSockIndex, const int msgid, const char* msg, const uint32_t msglen)
{
	
	Chat_ChatReply reply;
	reply.ParseFromArray(msg, msglen);
	LOG(INFO) << "client recv msg from gate :" << reply.chat_msg();
}


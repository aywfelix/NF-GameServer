#include "LoginNodeClient.h"
#include "ServerComm/Config.h"
#include "GameStruct/GameDef.h"

void LoginNodeClient::InitHelper()
{
    m_pNetClientModule->NFINetClientModule::AddEventCallBack(EServerType::SERVER_TYPE_MASTER, this, &LoginNodeClient::OnSocketEvent);
    m_pNetClientModule->NFINetClientModule::AddEventCallBack(EServerType::SERVER_TYPE_WORLD, this, &LoginNodeClient::OnSocketEvent);
	m_pNetClientModule->NFINetClientModule::AddEventCallBack(EServerType::SERVER_TYPE_GATE, this, &LoginNodeClient::OnSocketEvent);
	SetServerInfoReport();
	AddConnectServer();
}

void LoginNodeClient::SetServerInfoReport()
{
    m_serverInfoReport.set_server_id(g_pConfig->m_ServerConf["NodeId"].asInt());
    m_serverInfoReport.set_server_name(g_pConfig->m_ServerConf["NodeName"].asString());
    m_serverInfoReport.set_server_cur_count(0);
    m_serverInfoReport.set_server_ip(g_pConfig->m_ServerConf["NodeIp"].asString());
    m_serverInfoReport.set_server_port(g_pConfig->m_ServerConf["NodePort"].asInt());
    m_serverInfoReport.set_server_max_online(max_online);
    m_serverInfoReport.set_server_state(EServerState::EST_NORMAL);
    m_serverInfoReport.set_server_type(SERVER_TYPE_LOGIN);
}

void LoginNodeClient::AddConnectServer()
{
	AddConnectMaster();
    m_ConnectType.emplace_back(SERVER_TYPE_WORLD);
	m_ConnectType.emplace_back(SERVER_TYPE_GATE);
}


void LoginNodeClient::OnSocketEvent(const NFSOCK nSockIndex, const NF_NET_EVENT nEvent, NFINet* pNet)
{
    OnSocketNodeEvent(nSockIndex, nEvent, pNet);
}
#include "GameNodeClient.h"
#include "ServerComm/Config.h"

void GameNodeClient::InitHelper()
{
    m_pNetClientModule->NFINetClientModule::AddEventCallBack(EServerType::SERVER_TYPE_MASTER, this, &GameNodeClient::OnSocketEvent);
    m_pNetClientModule->NFINetClientModule::AddEventCallBack(EServerType::SERVER_TYPE_WORLD, this, &GameNodeClient::OnSocketEvent);
	m_pNetClientModule->NFINetClientModule::AddEventCallBack(EServerType::SERVER_TYPE_GATE, this, &GameNodeClient::OnSocketEvent);
	SetServerInfoReport();
	AddConnectServer();
}

void GameNodeClient::SetServerInfoReport()
{
    m_serverInfoReport.set_server_id(g_pConfig->m_ServerConf["NodeId"].asInt());
    m_serverInfoReport.set_server_name(g_pConfig->m_ServerConf["NodeName"].asString());
    m_serverInfoReport.set_server_cur_count(0);
    m_serverInfoReport.set_server_ip(g_pConfig->m_ServerConf["NodeIp"].asString());
    m_serverInfoReport.set_server_port(g_pConfig->m_ServerConf["NodePort"].asInt());
    m_serverInfoReport.set_server_max_online(2000);
    m_serverInfoReport.set_server_state(EServerState::EST_NORMAL);
    m_serverInfoReport.set_server_type(EServerType::SERVER_TYPE_GAME);
}

void GameNodeClient::AddConnectServer()
{
	AddConnectMaster();
    m_ConnectType.emplace_back(SERVER_TYPE_WORLD);
	m_ConnectType.emplace_back(SERVER_TYPE_GATE);
}


void GameNodeClient::OnSocketEvent(const NFSOCK nSockIndex, const NF_NET_EVENT nEvent, NFINet* pNet)
{
    OnSocketNodeEvent(nSockIndex, nEvent, pNet);
}
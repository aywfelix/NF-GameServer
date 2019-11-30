#include "WorldNodeClient.h"
#include "ServerComm/Config.h"
#include "GameStruct/GameDef.h"

void WorldNodeClient::InitHelper()
{
    m_pNetClientModule->NFINetClientModule::AddEventCallBack(EServerType::SERVER_TYPE_MASTER, this, &WorldNodeClient::OnSocketEvent);
	m_pNetClientModule->NFINetClientModule::AddEventCallBack(EServerType::SERVER_TYPE_GATE, this, &WorldNodeClient::OnSocketEvent);//world服连接master服务

	SetServerInfoReport();
	AddConnectServer();
}

void WorldNodeClient::SetServerInfoReport()
{
    m_serverInfoReport.set_server_id(g_pConfig->m_ServerConf["NodeId"].asInt());
    m_serverInfoReport.set_server_name(g_pConfig->m_ServerConf["NodeName"].asString());
    m_serverInfoReport.set_server_cur_count(0);
    m_serverInfoReport.set_server_ip(g_pConfig->m_ServerConf["NodeIp"].asString());
    m_serverInfoReport.set_server_port(g_pConfig->m_ServerConf["NodePort"].asInt());
    m_serverInfoReport.set_server_max_online(max_online);
    m_serverInfoReport.set_server_state(EServerState::EST_NORMAL);
    m_serverInfoReport.set_server_type(EServerType::SERVER_TYPE_WORLD);
}

void WorldNodeClient::AddConnectServer()
{
	AddConnectMaster();
	m_ConnectType.emplace_back(SERVER_TYPE_GATE);
}

void WorldNodeClient::OnSocketEvent(const NFSOCK nSockIndex, const NF_NET_EVENT nEvent, NFINet* pNet)
{
    OnSocketNodeEvent(nSockIndex, nEvent, pNet);
}


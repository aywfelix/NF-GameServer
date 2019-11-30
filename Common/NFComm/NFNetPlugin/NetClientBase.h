#pragma once

#include "NFComm/NFNetPlugin/NFNetClientModule.h"
#include <vector>

class NetClientBase
{
public:
	virtual bool Init();
	virtual void InitHelper() = 0;
	virtual void SetServerInfoReport() = 0;  
	virtual void AddConnectServer() = 0;
	virtual void OnMasterMessage(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	virtual bool Loop();

	virtual void OnSocketNodeEvent(const NFSOCK nSockIndex, const NF_NET_EVENT nEvent, NFINet* pNet);

	virtual NF_SHARE_PTR<ConnectData> GetServerNetInfo(const int nServerID);
	virtual NF_SHARE_PTR<ConnectData> GetServerNetInfo(const NFSOCK nSockIndex);

	virtual EServerType GetServerType()
	{
		return (EServerType)m_serverInfoReport.server_type();
	}
	virtual void AddConnectMaster();
protected:
	NFNetClientModule* m_pNetClientModule{ nullptr };
	ServerInfoReport m_serverInfoReport;
	std::vector<EServerType> m_ConnectType;
};

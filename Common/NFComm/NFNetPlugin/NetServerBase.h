#pragma once

#include "NFNetPlugin/NFNetModule.h"

class NetServerBase
{
public:
	virtual bool Init();
    virtual bool InitHelper() = 0;
    virtual bool Loop();
    virtual void OnClientSocketEvent(const NFSOCK nSockIndex, const NF_NET_EVENT nEvent, NFINet *pNet);
    virtual void OnClientConnected(const NFSOCK nSockIndex);
    virtual void OnClientDisconnect(const NFSOCK nSockIndex);
    NFNetModule* GetNetModule() {return m_pNetModule;}

    NF_SHARE_PTR<ServerData> GetClientNodeData(int nServerId);

    virtual void OnReportToServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	virtual void AfterReportToServer(NF_SHARE_PTR<ServerData> pReportServerData);

protected: 
    NFNetModule *m_pNetModule;
    NFMapEx<int, ServerData> m_mClientNodes;
};

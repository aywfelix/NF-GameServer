#pragma once
#include "NFComm/NFNetPlugin/NetClientBase.h"
#include "ServerComm/Config.h"

class DBNodeClient : public NetClientBase{
public:
    void InitHelper();
    void SetServerInfoReport();
    void AddConnectServer();
    void OnSocketEvent(const NFSOCK nSockIndex, const NF_NET_EVENT nEvent, NFINet* pNet);
   
private:
};
#pragma once

#include "./NFComm/NFNetPlugin/NetClientBase.h"


class GameNodeClient : public NetClientBase {
public:
    void InitHelper();
    void SetServerInfoReport();
    void AddConnectServer();
    void OnSocketEvent(const NFSOCK nSockIndex, const NF_NET_EVENT nEvent, NFINet* pNet);
    
private:
};
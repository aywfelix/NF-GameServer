#pragma once

#include "NFComm/NFNetPlugin/NetServerBase.h"

class DBNodeServer : public NetServerBase{
public:
    virtual bool InitHelper();
    virtual void OnClientDisconnect(const NFSOCK nSockIndex) {}
    void OtherMessage(const NFSOCK nSockIndex, const int nMsgID, const char *msg, const uint32_t nLen);

};
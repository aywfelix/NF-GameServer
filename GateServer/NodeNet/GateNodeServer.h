#pragma once

#include "NFComm/NFNetPlugin/NetServerBase.h"

//as server
class GateNodeServer : public NetServerBase
{
public:
    virtual bool InitHelper();
    virtual void OnClientDisconnect(const NFSOCK nSockIndex) {}
    void OtherMessage(const NFSOCK nSockIndex, const int msgid, const char *msg, const uint32_t nLen);

    //send msg to other server
    bool SendPackToLogin(NFSOCK client_fd, const int msgid, const char *msg, uint32_t nLen, int nGameID, const std::string &ip);
    bool SendPackToScene(const int msgid, const google::protobuf::Message &xData, int nServerID);
    bool SentPackToChat(const int msgid, const google::protobuf::Message &xData);
    bool SentPackToWorld(const int msgid, const google::protobuf::Message &xData);

    //back msg to client from other server
    void OnLoginRouteBack(NFSOCK nSockIndex, const int msgid, const char *msg, const uint32_t nLen);
    void OnChatRouteBack(NFSOCK nSockIndex, const int msgid, const char *msg, const uint32_t nLen);
    void OnWorldRouteBack(NFSOCK nSockIndex, const int msgid, const char *msg, const uint32_t nLen);
};
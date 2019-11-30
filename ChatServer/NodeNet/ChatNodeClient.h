#pragma once

#include "./NFComm/NFNetPlugin/NetClientBase.h"


class ChatNodeClient : public NetClientBase {
public:
    void InitHelper();
    void SetServerInfoReport();
    void AddConnectServer();
	void OnSocketEvent(const NFSOCK nSockIndex, const NF_NET_EVENT nEvent, NFINet* pNet);
    
	void OnGateRouteChat(const NFSOCK nSockIndex, const int msgid, const char* msg, const uint32_t msglen);
	void SendToGate(const int GameID, uint64_t playerId, uint32_t msg_id, ::google::protobuf::Message* pMsg);
private:
	//void AddConnectMaster();
};
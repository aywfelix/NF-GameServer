#pragma once

#include "NFComm/NFNetPlugin/NFNetClientModule.h"
#include <vector>


class Client
{
public:
	bool Init();
	void ConnectServer();
	bool Loop();

	void OnSocketEvent(const NFSOCK nSockIndex, const NF_NET_EVENT nEvent, NFINet* pNet);
	void OnMessage(const NFSOCK nSockIndex, const int msgid, const char* msg, const uint32_t msglen);

protected:
	NFNetClientModule* m_pNetClientModule{ nullptr };
	std::vector<EServerType> m_ConnectType;

	int m_ServerId;
};
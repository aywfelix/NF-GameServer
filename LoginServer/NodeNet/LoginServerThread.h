#pragma once
#include "ServerComm/ThreadBase.h"
#include "LoginNodeClient.h"


class LoginServerThread : public ThreadBase{
public:
    bool Init();
    void ThreadLoop();

	LoginNodeClient& LoginClient()
	{
		return m_LoginClient;
	}
private:
	LoginNodeClient m_LoginClient;
};

extern std::unique_ptr<LoginServerThread> g_pServerThread;
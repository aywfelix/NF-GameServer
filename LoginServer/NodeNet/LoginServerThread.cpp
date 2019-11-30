
#include "LoginServerThread.h"
#include "ServerComm/Util.h"

std::unique_ptr<LoginServerThread> g_pServerThread = nullptr;

bool LoginServerThread::Init()
{
	m_LoginClient.Init();
	m_LoginClient.InitHelper();
	return true;
}

void LoginServerThread::ThreadLoop()
{
	while (IsActive())
	{
		m_LoginClient.Loop();   //net loop
		MySleep(10);
	}
	
}


#include "ChatServerThread.h"
#include "ServerComm/Util.h"

std::unique_ptr<ChatServerThread> g_pServerThread = nullptr;

bool ChatServerThread::Init()
{
	m_ChatClient.Init();
	m_ChatClient.InitHelper();
	return true;
}

void ChatServerThread::ThreadLoop()
{
	while (IsActive())
	{
		m_ChatClient.Loop();
		MySleep(10);
	}
}

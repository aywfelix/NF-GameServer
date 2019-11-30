#include "GameServerThread.h"
#include "ServerComm/Util.h"

std::unique_ptr<GameServerThread> g_pServerThread = nullptr;

bool GameServerThread::Init()
{
	m_GameClient.Init();
	m_GameClient.InitHelper();
	return true;
}

void GameServerThread::ThreadLoop()
{
	while (IsActive())
	{
		m_GameClient.Loop();
		MySleep(10);
	}
	
}

#include "NodeNet/WorldServerThread.h"
#include "ServerComm/Util.h"

std::unique_ptr<WorldServerThread> g_pServerThread = nullptr;
bool WorldServerThread::Init()
{
    m_NodeServer.Init();
    m_NodeServer.InitHelper();
	return true;
}

void WorldServerThread::ThreadLoop()
{
    while(IsActive())
    {
        m_NodeServer.Loop();
		MySleep(10);
    }
}

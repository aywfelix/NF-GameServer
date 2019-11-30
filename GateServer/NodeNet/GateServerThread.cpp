#include "NodeNet/GateServerThread.h"
#include "ServerComm/Util.h"

std::unique_ptr<GateServerThread> g_pServerThread = nullptr;
bool GateServerThread::Init()
{
    m_NodeServer.Init();
    m_NodeServer.InitHelper();

	m_NodeClient.Init();
	m_NodeClient.InitHelper();

	m_PlayerServer.Init();

	return true;
}

void GateServerThread::ThreadLoop()
{
    while(IsActive())
    {
        m_NodeServer.Loop();
		m_NodeClient.Loop();
		m_PlayerServer.Loop();
		MySleep(10);
    }
}

#include "MasterServerThread.h"
#include "ServerComm/Util.h"

std::unique_ptr<MasterServerThread> g_pServerThread = nullptr;

bool MasterServerThread::Init()
{
    m_NodeServer.Init();
    m_NodeServer.InitHelper();
	return true;
}

void MasterServerThread::ThreadLoop()
{
    while(IsActive())
    {
        m_NodeServer.Loop();
		MySleep(10);
    }
}

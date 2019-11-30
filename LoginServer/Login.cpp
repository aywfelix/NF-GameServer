#include "Login.h"
#include "NodeNet/LoginServerThread.h"


Login::Login()
{

}
Login::~Login()
{

}

void Login::Init()
{
	g_pServerThread.reset(new LoginServerThread());

	InitManager();
}

void Login::Start()
{
	g_pServerThread->Start();
}

void Login::Stop()
{
	g_pServerThread->Stop();
}

void Login::InitManager()
{

}

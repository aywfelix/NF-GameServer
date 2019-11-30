#include "ServerComm/Config.h"
#include "ServerComm/Util.h"
#include "easylogging/easylogging++.h"
#include "Chat.h"
#include <signal.h>

INITIALIZE_EASYLOGGINGPP

bool bStopServer = false;

void OnSignal(int sig)
{
	switch (sig)
	{
	case SIGINT:
	case SIGTERM:
#ifdef __WINDOWS__
	case SIGBREAK:
#else
	case SIGPIPE:
#endif
	{
		LOG(ERROR) << "Master Server Stop !!!, signal=" << sig;
		bStopServer = true;
		break;
	}
	default:
		break;
	}
}

void OnHookSignal()
{
	signal(SIGINT, OnSignal);
	signal(SIGTERM, OnSignal);
#ifdef __WINDOWS__
	signal(SIGBREAK, OnSignal);
#else
	signal(SIGPIPE, OnSignal);
#endif
}


int main()
{
	OnHookSignal();
	el::Configurations conf("../Config/chat_log.conf");
	el::Loggers::reconfigureAllLoggers(conf);

    g_pConfig.reset(new Config());
    g_pConfig->Load("../Config/server_conf.json");
    g_pConfig->m_ServerConf = g_pConfig->m_Root["ChatServer"];   

	Chat chat;
	chat.Init();
	chat.Start();

	while (bStopServer == false)
	{
		MySleep(500);
	}

	chat.Stop();

    return 0;
}

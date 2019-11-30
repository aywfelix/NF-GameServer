#include "ServerComm/Config.h"
#include "ServerComm/Util.h"
#include "easylogging/easylogging++.h"
#include "Client.h"

INITIALIZE_EASYLOGGINGPP

int main()
{
	el::Configurations conf("../Config/global_log.conf");
	el::Loggers::reconfigureLogger("default", conf);

	//init config
	g_pConfig.reset(new Config);
	g_pConfig->Load("../Config/server_conf.json");
	g_pConfig->m_ServerConf = g_pConfig->m_Root["GateServer"];

	Client client;
	client.Init();
	while (true)
	{
		client.Loop();
		MySleep(500);
	}
	return 0;
}
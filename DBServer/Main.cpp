#include "NFPluginModule/NFPlatform.h"
#include "easylogging/easylogging++.h"
#include "DB.h"
#include "ServerComm/Config.h"
#include "ServerComm/Util.h"

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
	el::Configurations conf("../Config/db_log.conf");
	el::Loggers::reconfigureAllLoggers(conf);

	//server config
	g_pConfig.reset(new Config());
	g_pConfig->Load("../Config/server_conf.json");
	g_pConfig->m_ServerConf = g_pConfig->m_Root["DBServer"];
	//mariadb config
	g_pConfig->m_dbConf = g_pConfig->m_Root["MariaDB"];


	//scoped_library_init maria_lib_init;



	//try {
	//	Connection conn;
	//	

	//	conn.query("CREATE TABLE test2 "
	//		"(i INT, s CHAR(15), d DATETIME)");
	//	std::auto_ptr<PreparedStatement> stmt(
	//		conn.prepare("INSERT INTO test2 (i,s,d) values(?,?,?)"));

	//	assert(3 == stmt->param_count());
	//	stmt->setInt(0, 1);
	//	stmt->setString(1, "string-1");
	//	stmt->setDateTime(2, Time("2016-03-23 02:41"));
	//	stmt->execute();

	//	stmt->setInt(0, 2);
	//	stmt->setNull(1);
	//	stmt->setDateTime(2, Time::datetime(2015, 02, 21, 12, 45, 51));
	//	stmt->execute();

	//	stmt.reset(conn.prepare("SELECT i, s, d FROM test2 ORDER BY i"));
	//	stmt->execute();
	//	while (stmt->fetch()) {
	//		std::cout << "i = " << stmt->getInt(0);
	//		std::cout << ", s = ";
	//		if (stmt->isNull(1)) std::cout << "NULL";
	//		else std::cout << stmt->getString(1);
	//		std::cout << ", d = " << stmt->getTime(2);
	//		std::cout << std::endl;
	//	}
	//	conn.close();
	//}
	//catch (Exception & e) {
	//	std::cerr << e << std::endl;
	//	return 1;
	//}
	//return 0;




	DB db;
	db.Init();
	db.Start();

	while (bStopServer == false)
	{
		MySleep(500);
	}

	db.Stop();

	return 0;
}
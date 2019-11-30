#include "TestRedis.h"
#include <iostream>
using namespace std;

TestRedis::TestRedis()
{
	g_pGameRedis.reset(new GameRedis);
}

bool TestRedis::TestConnectRedis()
{
	if (g_pGameRedis->ConnectRedis("192.168.40.182", 6379))
	{
		cout << "connec redis ok" << endl;
		return true;
	}
	cout << "connec redis error" << endl;
	return false;
}

bool TestRedis::SetString()
{
	if (g_pGameRedis->SetString("teststring", "xxxxxxxxxxxxxxxxxxx"))
	{
		cout << "SetString redis ok" << endl;
	}
	if (g_pGameRedis->IsExist("teststring"))
	{
		cout << "SetString redis ok!!!!!!!!!!" << endl;
		return true;
	}
	return false;
}
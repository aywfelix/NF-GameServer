#include <vector>
#include <memory>
#include <iostream>
#include "TestRedis.h"
#include <WinSock2.h>
using namespace std;



int main()
{
	WSADATA wsaData;
	int nRet;
	if ((nRet = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		printf("WSAStartup failed\n");
		exit(0);
	}
	TestRedis myRedis;
	myRedis.TestConnectRedis();
	myRedis.SetString();
	return 0;
}
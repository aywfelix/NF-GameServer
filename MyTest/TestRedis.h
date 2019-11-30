#pragma once

#include "redis/GameRedis.h"

class TestRedis
{
public:
	TestRedis();
	bool TestConnectRedis();
	bool SetString();
};


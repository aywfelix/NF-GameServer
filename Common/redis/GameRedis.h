#pragma once

#include <string>
#include <memory>

class RedisOp;

class GameRedis
{
public:
	GameRedis();
	~GameRedis();

	bool ConnectRedis(const char* ip, unsigned int port);
	bool ConnectRedis();

	bool SetString(const char* key, const char* value);
	bool IsExist(const char* key);
	void Stop();
private:
	RedisOp* m_RedisOp;
	std::string m_ip;
	unsigned int m_port;
};

extern std::unique_ptr<GameRedis> g_pGameRedis;


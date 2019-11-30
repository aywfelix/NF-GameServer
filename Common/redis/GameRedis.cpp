#include "GameRedis.h"
#include "redis/RedisOp.h"

std::unique_ptr<GameRedis> g_pGameRedis = nullptr;

GameRedis::GameRedis()
{
	m_RedisOp = new RedisOp;
}

GameRedis::~GameRedis()
{
}

bool GameRedis::ConnectRedis(const char* ip, unsigned int port)
{
	m_ip = ip;
	m_port = port;
	return ConnectRedis();
}

bool GameRedis::ConnectRedis()
{
	return m_RedisOp->ConnectRedis(m_ip.c_str(), m_port);
}

bool GameRedis::SetString(const char* key, const char* value)
{
	return m_RedisOp->SetString(key, value);
}

bool GameRedis::IsExist(const char* key)
{
	return m_RedisOp->IsExist(key);
}

void GameRedis::Stop()
{
	m_RedisOp->FreeRedis();
}
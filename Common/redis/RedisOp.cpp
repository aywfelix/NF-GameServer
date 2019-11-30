#include "RedisOp.h"
#include "ServerComm/lexical_cast.hpp"
#include "ServerComm/Util.h"
#include "easylogging/easylogging++.h"
#include <cmath>


RedisOp::RedisOp()
{
}


RedisOp::~RedisOp()
{
	FreeRedis();
}

bool RedisOp::IsConnect()
{
	if (m_redis == nullptr)
	{
		return false;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "PING"));
	if (m_reply == nullptr)
	{
		return false;
	}
	AutoDelete<redisReply> autoRelease(m_reply);
	if (m_reply->type == REDIS_REPLY_ERROR)
	{
		return false;
	}
	std::string strLeft = std::string(m_reply->str);
	if (strLeft != std::string("PONG"))
	{
		return false;
	}

	return true;
}

bool RedisOp::ConnectRedis(const char* ip, unsigned int port)
{
	FreeRedis();
	struct timeval timeout = { 2, 500000 };
	m_redis = redisConnectWithTimeout(ip, port, timeout);
	if (m_redis == nullptr || m_redis->err)
	{
		LOG(ERROR) << "Connect Redis Server failed, ip:" << ip << " port:" << port;
		LOG(ERROR) << m_redis->errstr;
		return false;
	}
	return true;
}

void  RedisOp::FreeRedis()
{
	if (m_redis)
	{
		redisFree(m_redis);
		m_redis = nullptr;
	}
}

bool RedisOp::SetKeyExpire(const char* key, int iTimeOut)
{
	if (m_redis == nullptr)
		return false;
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "expire %s %d", key, iTimeOut));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return false;
	}

	int nRet = (int)m_reply->integer;
	return nRet == 1;
}

bool RedisOp::IsExist(const char* key)
{
	if (m_redis == nullptr)
		return false;

	int rec = -3;
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "EXISTS %s", key));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return false;
	}
	rec = (int)m_reply->integer;
	return rec == 1;

}

bool RedisOp::DeleteKey(const char* key)
{

	if (m_redis == nullptr)
		return false;

	int rec = -3;
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "DEL %s", key));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		if (m_reply)
		{
			rec = (int)m_reply->integer;
		}
		return false;
	}

	return rec == 1;

}

bool RedisOp::SetString(const char* key, const char* val)
{

	if (m_redis == nullptr)return false;

	int rec = -3;
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "SET %s %s", key, val));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (m_reply)
	{
		LOG(ERROR) << m_reply->str;
	}
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return false;
	}
	return true;

}

std::string	RedisOp::GetString(const char* key)
{

	if (m_redis == nullptr)return "";
	std::string res;
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "GET %s", key));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return "";
	}
	if (nullptr != m_reply && m_reply->str != nullptr && m_reply->len > 0)
	{
		res = m_reply->str;
	}
	return res;

}


bool  RedisOp::GetMsgData(const char* key, google::protobuf::Message* pdata)
{

	if (m_redis == nullptr)
		return false;

	if (nullptr == key || nullptr == pdata)
		return false;
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "GET %s", key));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return false;
	}
	if (m_reply->type == REDIS_REPLY_NIL)
		return true;

	pdata->ParseFromString(m_reply->str);

	return true;

}


bool RedisOp::SetMsgData(const char* key, google::protobuf::Message* pdata)
{

	if (m_redis == nullptr)
		return false;

	if (!(nullptr != key && nullptr != pdata))
		return false;
	std::string buffer = pdata->SerializeAsString();
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "SET %s %s", key, buffer.c_str()));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return false;
	}
	bool bRet = false;
	if (strcmp(m_reply->str, "OK") == 0)
		bRet = true;
	return bRet;

}

int	RedisOp::AddHashStringValue(const char* key, const char* filed, const char* value)
{
	// 实现之后，把原来的散列存储的方式修改掉
	if (m_redis == nullptr)
	{
		return -1;
	}

	if (nullptr == key || nullptr == filed || nullptr == value || strlen(key) <= 0 || strlen(filed) <= 0 || strlen(value) <= 0)
		return -1;

	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HSET %s %s %s", key, filed, value));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}

	return 0;

}
int	RedisOp::GetHashStringValue(const char* key, const char* filed, std::string& value)
{

	if (m_redis == nullptr)
		return -1;

	if (nullptr == key || nullptr == filed || strlen(key) <= 0 || strlen(filed) <= 0)
		return -1;
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HGET %s %s", key, filed));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}
	if (m_reply->str == nullptr)
	{
		return -1;
	}

	value = m_reply->str;
	return 0;

}
int	RedisOp::DelHashStringValue(const char* key, const char* filed)
{

	if (m_redis == nullptr)
		return -1;

	if (nullptr == key || nullptr == filed || strlen(key) <= 0 || strlen(filed) <= 0)
		return -1;
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HDEL %s %s", key, filed));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}

	return 0;
}

int RedisOp::GetHashStringLength(const char* key, int32_t& nCount)
{
	nCount = 0;
	if (m_redis == nullptr)
		return -1;
	if (key == nullptr || strlen(key) <= 0)
	{
		return -1;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HLEN %s", key));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}
	nCount = m_reply->integer;
	return 0;
}

int RedisOp::ExistsHashFiled(const char* key, const char* filed, bool& bExists)
{
	bExists = false;
	if (m_redis == nullptr)
		return -1;
	if (key == nullptr || strlen(key) <= 0)
	{
		return -1;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HEXISTS %s %s", key, filed));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}
	bExists = (m_reply->integer == 1);
	return 0;
}

int	RedisOp::GetHashFileds(const char* key, std::vector<std::string>& fileds)
{

	if (m_redis == nullptr)
		return -1;

	if (nullptr == key || strlen(key) <= 0)
		return -1;

	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HKEYS %s", key));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}

	for (std::size_t i = 0; i < m_reply->elements; ++i)
	{
		redisReply* temp = m_reply->element[i];
		fileds.push_back(temp->str);
	}
	return 0;
}

int	RedisOp::AddHashBinaryValue(const char* key, const char* filed, const char* value, int nLen)
{

	if (m_redis == nullptr)
		return -1;

	if (nullptr == key || nullptr == filed || strlen(key) <= 0 || strlen(filed) <= 0)
		return -1;

	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HSET %s %s %b", key, filed, value, nLen));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}

	return 0;

}
int	RedisOp::GetHashBinaryValue(const char* key, const char* filed, char* value, int& nDataLen)
{

	if (m_redis == nullptr)
		return -1;

	if (nullptr == key || nullptr == filed || strlen(key) <= 0 || strlen(filed) <= 0)
		return -1;

	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "HGET %s %s", key, filed));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (m_reply == nullptr)
	{
		return -1;
	}
	if (m_reply->len < 0)
		return -1;
	nDataLen = m_reply->len;
	memcpy(value, m_reply->str, m_reply->len);

	return 0;

}

int RedisOp::AddHashBinaryValueEx(const char* key, const char* filed, const char* value, int nLen)
{

	if (m_redis == nullptr)
		return -1;

	if (nullptr == key || nullptr == filed || strlen(key) <= 0 || strlen(filed) <= 0 || nLen <= 0)
		return -1;
	const char* v[4];
	size_t vlen[4];
	v[0] = "HSET";
	v[1] = key;
	v[2] = filed;
	v[3] = value;

	vlen[0] = strlen("HSET");
	vlen[1] = strlen(key);
	vlen[2] = strlen(filed);
	vlen[3] = nLen;

	m_reply = reinterpret_cast<redisReply*>(redisCommandArgv(m_redis, 4, v, vlen));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}

	return 0;
}


int	RedisOp::AddSetStringValue(const char* key, const char* value)
{

	if (m_redis == nullptr)
	{
		return -1;
	}
	if (nullptr == key || nullptr == value || strlen(value) <= 0 || strlen(key) <= 0)
	{
		return -1;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "SADD %s %s", key, value));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}
	if (m_reply->integer == 0) // 也有可能是已经有了
		return 1;
	return 0;
}

int	RedisOp::GetAndRemoveSetStringValue(const char* key, std::string& value)
{

	if (m_redis == nullptr)
		return -1;
	if (nullptr == key || strlen(key) <= 0)
		return -1;
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "SPOP %s", key));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}
	if (m_reply->len > 0)
		value = std::string(m_reply->str);
	return 0;

}

//redis sort set
#define BIG (2<<32 - 2<<31)     //保存偏移量 math.pow(2, 32)
#define MAX_TM 2<<31  //最大时间 math.pow(2, 31)
int RedisOp::ZAdd(const char* key, int score, const char* member)
{
	if (m_redis == nullptr)
		return -1;
	if (nullptr == key || strlen(key) <= 0)
	{
		return -1;
	}
	if (nullptr == member || strlen(member) <= 0)
	{
		return -1;
	}
	/**
	 * score最大表示 到 int 最大值
	 * 0x7fffffff int最大值
	 * time(0) resutl int
	 */

	int64_t nCopyScore = score;
	int64_t nScore = (nCopyScore << 32) | (0x7FFFFFFF - time(0));
	double fScore = nScore;
	//INT64 nScore = score * BIG + (MAX_TM - time(0)); 
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "ZADD %s %f %s", key, fScore, member));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}
	return 0;
}

int RedisOp::ZAddUpdate(const char* key, int score, const char* member)
{
	if (m_redis == nullptr)
		return -1;
	if (nullptr == key || strlen(key) <= 0)
	{
		return -1;
	}
	if (nullptr == member || strlen(member) <= 0)
	{
		return -1;
	}
	int64_t nCopyScore = score;
	int64_t nScore = (nCopyScore << 32) | (0x7FFFFFFF - time(0));//仅仅更新存在的成员，不添加新成员
	double fScore = nScore;
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "ZADD %s  %f %s", key, fScore, member));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}
	return 0;
}

int RedisOp::ZCard(const char* key)
{
	if (m_redis == nullptr)
		return -1;
	if (nullptr == key || strlen(key) <= 0)
	{
		return -1;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "ZCARD %s", key));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}
	return m_reply->integer;
}

//int RedisOp::ZIncrby(const char* key, int add, const char* member)
//{
//	if (m_redis == nullptr)
//		return -1;
//	if (nullptr == key || strlen(key) <= 0)
//	{
//		return -1;
//	}
//	if (nullptr == member || strlen(member) <= 0)
//	{
//		return -1;
//	}
//	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "ZINCRBY %s %d %s", key, add, member));
//	HelpReleaseReply autoRelease(m_reply);
//	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
//	{
//		ELOG("redisCommand reply error");
//		return -1;
//	}
//	return 0;
//}

int RedisOp::ZRange(const char* key, int minIdx, int maxIdx, std::vector<std::string>& members)
{
	if (m_redis == nullptr)
		return -1;
	if (nullptr == key || strlen(key) <= 0)
	{
		return -1;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "ZRANGE %s %d %d", key, minIdx - 1, maxIdx - 1));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}
	for (size_t i = 0; i < m_reply->elements; ++i)
	{
		redisReply* pReply = m_reply->element[i];
		if (pReply && pReply->len > 0)
		{
			members.emplace_back(pReply->str);
		}
	}
	return 0;
}

int RedisOp::ZRangeWithScores(const char* key, int minIdx, int maxIdx, std::vector<std::pair<std::string, int>>& members)
{
	if (m_redis == nullptr)
		return -1;
	if (nullptr == key || strlen(key) <= 0)
	{
		return -1;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "ZRANGE %s %d %d WITHSCORES", key, minIdx - 1, maxIdx - 1));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}
	for (size_t i = 0; i < m_reply->elements; i += 2)
	{
		int n = i;
		redisReply* pReplyMember = m_reply->element[n];
		redisReply* pReplyScore = m_reply->element[++n];
		if (pReplyMember && pReplyMember->len > 0 && pReplyScore && pReplyScore->len > 0)
		{
			const char* pMember = pReplyMember->str;
			double score = lexical_cast<double>(pReplyScore->str);
			int64_t value = score;
			int nValue = value >> 32;
			members.emplace_back(std::pair<std::string, int>(pMember, nValue));
		}
	}
	return 0;
}

int RedisOp::ZRevRange(const char* key, int minIdx, int maxIdx, std::vector<std::string>& members)
{
	if (m_redis == nullptr)
		return -1;
	if (nullptr == key || strlen(key) <= 0)
	{
		return -1;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "ZREVRANGE %s %d %d", key, minIdx - 1, maxIdx - 1));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}
	for (size_t i = 0; i < m_reply->elements; ++i)
	{
		redisReply* pReply = m_reply->element[i];
		if (pReply && pReply->len > 0)
		{
			members.emplace_back(pReply->str);
		}
	}
	return 0;
}

int RedisOp::ZRevRangeWithScores(const char* key, int minIdx, int maxIdx, std::vector<std::pair<std::string, int>>& members)
{
	if (m_redis == nullptr)
		return -1;
	if (nullptr == key || strlen(key) <= 0)
	{
		return -1;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "ZREVRANGE %s %d %d WITHSCORES", key, minIdx - 1, maxIdx - 1));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}
	for (size_t i = 0; i < m_reply->elements; i += 2)
	{
		int n = i;
		redisReply* pReplyMember = m_reply->element[n];
		redisReply* pReplyScore = m_reply->element[++n];
		if (pReplyMember && pReplyMember->len > 0 && pReplyScore && pReplyScore->len > 0)
		{
			const char* pMember = pReplyMember->str;
			double score = lexical_cast<double>(pReplyScore->str);
			int64_t value = score;
			int nValue = value >> 32;
			members.emplace_back(std::pair<std::string, int>(pMember, nValue));
		}
	}
	return 0;
}

int RedisOp::ZRank(const char* key, const char* member)
{
	if (m_redis == nullptr)
		return -1;
	if (nullptr == key || strlen(key) <= 0)
	{
		return -1;
	}
	if (nullptr == member || strlen(member) <= 0)
	{
		return -1;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "ZRANK %s %s", key, member));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR || m_reply->type == REDIS_REPLY_NIL)
	{
		return -1;
	}
	return (m_reply->integer + 1);
}

int RedisOp::ZRevRank(const char* key, const char* member)
{
	if (m_redis == nullptr)
		return -1;
	if (nullptr == key || strlen(key) <= 0)
	{
		return -1;
	}
	if (nullptr == member || strlen(member) <= 0)
	{
		return -1;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "ZREVRANK %s %s", key, member));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}
	if (m_reply->type == REDIS_REPLY_NIL)
	{
		return -1;
	}
	return (m_reply->integer + 1);
}

int RedisOp::ZRem(const char* key, const char* member)
{
	if (m_redis == nullptr)
		return -1;
	if (nullptr == key || strlen(key) <= 0)
	{
		return -1;
	}
	if (nullptr == member || strlen(member) <= 0)
	{
		return -1;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "ZREM %s %s", key, member));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}
	return 0;
}

int RedisOp::ZRemRangeByRank(const char* key, int nFrom, int nTo)
{
	if (m_redis == nullptr)
		return -1;
	if (nullptr == key || strlen(key) <= 0)
	{
		return -1;
	}

	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "ZREMRANGEBYRANK %s %d %d", key, nFrom, nTo));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}
	return 0;
}

int RedisOp::ZScore(const char* key, const char* member)
{
	if (m_redis == nullptr)
		return -1;
	if (nullptr == key || strlen(key) <= 0)
	{
		return -1;
	}
	if (nullptr == member || strlen(member) <= 0)
	{
		return -1;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "ZSCORE %s %s", key, member));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}
	int nValue = 0;
	if (m_reply->len > 0)
	{
		//score =  std::atoll(m_reply->str);
		double score = lexical_cast<double>(m_reply->str);
		int64_t value = score;
		nValue = value >> 32;
	}
	return nValue;
}

int RedisOp::ZDel(const char* key)
{
	if (m_redis == nullptr)
		return -1;
	if (nullptr == key || strlen(key) <= 0)
	{
		return -1;
	}
	m_reply = reinterpret_cast<redisReply*>(redisCommand(m_redis, "DEL %s", key));
	AutoDelete<redisReply> autoRelease(m_reply);
	if (nullptr == m_reply || m_reply->type == REDIS_REPLY_ERROR)
	{
		return -1;
	}
	return 0;
}
#pragma once

#include <hiredis.h>
#include <string>
#include <google/protobuf/message.h>

class RedisOp
{
public:
	RedisOp();
	virtual ~RedisOp();
	
	bool ConnectRedis(const char* ip, unsigned int port);
	bool IsConnect();
	void FreeRedis();
	bool SetKeyExpire(const char* key, int iTimeOut);
	bool IsExist(const char* key);
	bool DeleteKey(const char* key);
	bool SetString(const char* key, const char* val);
	std::string	GetString(const char* key);
	bool GetMsgData(const char* key, google::protobuf::Message* pdata);
	bool SetMsgData(const char* key, google::protobuf::Message* pdata);
	int	AddHashStringValue(const char* key, const char* filed, const char* value);
	int	GetHashStringValue(const char* key, const char* filed, std::string& value);
	int	DelHashStringValue(const char* key, const char* filed);
	int	GetHashStringLength(const char* key, int32_t& nCount);
	int	ExistsHashFiled(const char* key, const char* filed, bool& bExists);
	int	GetHashFileds(const char* key, std::vector<std::string>& fileds);
	int	AddHashBinaryValue(const char* key, const char* filed, const char* value, int nLen);
	int	GetHashBinaryValue(const char* key, const char* filed, char* value, int& nDataLen);
	int	AddHashBinaryValueEx(const char* key, const char* filed, const char* value, int nLen);

	int	AddSetStringValue(const char* key, const char* value);
	int	GetAndRemoveSetStringValue(const char* key, std::string& value);

	int ZAdd(const char* key, int score, const char* member);
	int ZAddUpdate(const char* key, int score, const char* member);
	int ZCard(const char* key);
	//int ZIncrby(const char* key, int add, const char* member);// 如果key中不存在member，就在key中添加一个member，score是add
	int ZRange(const char* key, int minIdx, int maxIdx, std::vector<std::string>& members);
	int ZRangeWithScores(const char* key, int minIdx, int maxIdx, std::vector<std::pair<std::string, int>>& members);
	int ZRevRange(const char* key, int minIdx, int maxIdx, std::vector<std::string>& members);
	int ZRevRangeWithScores(const char* key, int minIdx, int maxIdx, std::vector<std::pair<std::string, int>>& members);
	int ZRank(const char* key, const char* member); // 返回有序集key中成员member的排名,score从小到大
	int ZRevRank(const char* key, const char* member);
	int ZRem(const char* key, const char* member);
	int ZRemRangeByRank(const char* key, int nFrom, int nTo);
	int ZScore(const char* key, const char* member);
	int ZDel(const char* key);

	
private:
	redisContext* m_redis{nullptr};
	redisReply* m_reply{nullptr};
};



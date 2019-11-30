#pragma once

#include "jsoncpp/json.h"
#include <memory>


class Config {
public:
	bool Load(const char* jsonFile);

	Json::Value m_Root;
	Json::Value m_ServerConf;

	Json::Value m_dbConf;
};

extern std::unique_ptr<Config>  g_pConfig;
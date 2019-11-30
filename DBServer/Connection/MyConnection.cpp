#include "MyConnection.h"
#include "mariacpp/exception.hpp"
#include "mariacpp/prepared_stmt.hpp"
#include "mariacpp/time.hpp"
#include "mariacpp/uri.hpp"
#include "ServerComm/Config.h"
#include "easylogging/easylogging++.h"
//#include <cstdlib>
//#include <iostream>
//#include <memory>

MyConnection::MyConnection()
{
}

MyConnection::~MyConnection()
{
}


bool MyConnection::ConnectToDB()
{
	const char* uri = g_pConfig->m_dbConf["uri"].asCString();
	const char* user = g_pConfig->m_dbConf["root"].asCString();
	const char* passwd = g_pConfig->m_dbConf["passwd"].asCString();
	try
	{
		m_Conn.connect(MariaCpp::Uri(uri), user, passwd);
	}
	catch (const std::exception & e)
	{
		LOG(ERROR) << "connect mariadb error," << e.what();
		return false;
	}
	return true;
}

bool MyConnection::IsConnOk()
{
	if (m_Conn.MyPing() != 0)
	{
		return false;
	}
	return true;
}

void MyConnection::DisConnect()
{
	m_Conn.close();
}

bool MyConnection::ExecuteSql(const std::string& sql)
{
	m_Conn.query(sql);
	return true;
}
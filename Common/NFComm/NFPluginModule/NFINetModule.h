/*
 * @Description: In User Settings Edit
 * @Author: your name
 * @Date: 2019-09-27 15:24:20
 * @LastEditTime: 2019-09-27 17:52:32
 * @LastEditors: Please set LastEditors
 */
/*
            This file is part of: 
                NoahFrame
            https://github.com/ketoo/NoahGameFrame

   Copyright 2009 - 2019 NoahFrame(NoahGameFrame)

   File creator: lvsheng.huang
   
   NoahFrame is open-source software and you can redistribute it and/or modify
   it under the terms of the License; besides, anyone who use this file/software must include this copyright announcement.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef NFI_NET_MODULE_H
#define NFI_NET_MODULE_H

#include <iostream>
#include <iosfwd>
#include "NFGUID.h"
#include "NFComm/NFNetPlugin/NFINet.h"
#include "NFComm/NFCore/NFQueue.hpp"
#include "NFIModule.h"
#ifdef _MSC_VER
#pragma warning(disable: 4244 4267)
#endif
#include "ServerProto/NFNodeNet.pb.h"
#include "ServerProto/ServerPublic.pb.h"

using namespace NFNetProto;


////////////////////////////////////////////////////////////////////////////

//only use this macro when u has entered game server
#define CLIENT_MSG_PROCESS(nMsgID, msgData, nLen, msg)                 \
    NFGUID nPlayerID;                                \
    msg xMsg;                                           \
    if (!NFINetModule::ReceivePB(nMsgID, msgData, nLen, xMsg, nPlayerID))             \
    {                                                   \
        m_pLogModule->LogNormal(NFILogModule::NLL_ERROR_NORMAL, NFGUID(), "", "Parse msg error", __FUNCTION__, __LINE__); \
        return;                                         \
    }                                                   \
    \
    NF_SHARE_PTR<NFIObject> pObject = m_pKernelModule->GetObject(nPlayerID); \
    if ( NULL == pObject.get() )                        \
    {                                                   \
        m_pLogModule->LogNormal(NFILogModule::NLL_ERROR_NORMAL, nPlayerID, "FromClient Object do not Exist", "", __FUNCTION__, __LINE__); \
        return;                                         \
    }

#define CLIENT_MSG_PROCESS_NO_OBJECT(nMsgID, msgData, nLen, msg)                 \
    NFGUID nPlayerID;                                \
    msg xMsg;                                           \
    if (!NFINetModule::ReceivePB(nMsgID, msgData, nLen, xMsg, nPlayerID))             \
    {                                                   \
        m_pLogModule->LogNormal(NFILogModule::NLL_ERROR_NORMAL, NFGUID(), "", "Parse msg error", __FUNCTION__, __LINE__); \
        return;                                         \
    }

#define CLIENT_MSG_PROCESS_NO_LOG(nMsgID, msgData, nLen, msg)                 \
    NFGUID nPlayerID;                                \
    msg xMsg;                                           \
    if (!NFINetModule::ReceivePB(nMsgID, msgData, nLen, xMsg, nPlayerID))             \
    {                                                   \
        return 0;                                         \
    }

//////////////////////////////////////////////////////////////////////////
struct ServerData
{
    ServerData()
    {
        pData = NF_SHARE_PTR<NFNetProto::ServerInfoReport>(NF_NEW NFNetProto::ServerInfoReport());
        nFD = 0;
    }
    ~ServerData()
    {
        nFD = 0;
        pData = NULL;
    }

	NFSOCK nFD;
    NF_SHARE_PTR<NFNetProto::ServerInfoReport> pData;
};

class NFINetModule : public NFIModule
{
public:
	static NFGUID PBToNF(Ident xID)
	{
		NFGUID  xIdent;
		xIdent.nHead64 = xID.svrid();
		xIdent.nData64 = xID.index();

		return xIdent;
	}

	static NFVector2 PBToNF(Vector2 value)
	{
		NFVector2  vector;
		vector.SetX(value.x());
		vector.SetY(value.y());
		return vector;
	}

	static NFVector3 PBToNF(Vector3 value)
	{
		NFVector3  vector;
		vector.SetX(value.x());
		vector.SetY(value.y());
		vector.SetZ(value.z());
		return vector;
	}

	static Ident NFToPB(NFGUID xID)
	{
		Ident  xIdent;
		xIdent.set_svrid(xID.nHead64);
		xIdent.set_index(xID.nData64);

		return xIdent;
	}

	static Vector2 NFToPB(NFVector2 value)
	{
		Vector2  vector;
		vector.set_x(value.X());
		vector.set_y(value.Y());
		return vector;
	}

	static Vector3 NFToPB(NFVector3 value)
	{
		Vector3  vector;
		vector.set_x(value.X());
		vector.set_y(value.Y());
		vector.set_z(value.Z());
		return vector;
	}

	template<typename BaseType>
	bool AddReceiveCallBack(const int nMsgID, BaseType* pBase, void (BaseType::*handleRecieve)(const NFSOCK, const int, const char*, const uint32_t))
	{
		NET_RECEIVE_FUNCTOR functor = std::bind(handleRecieve, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
		NET_RECEIVE_FUNCTOR_PTR functorPtr(new NET_RECEIVE_FUNCTOR(functor));

		return AddReceiveCallBack(nMsgID, functorPtr);
	}

	template<typename BaseType>
	bool AddReceiveCallBack(BaseType* pBase, void (BaseType::*handleRecieve)(const NFSOCK, const int, const char*, const uint32_t))
	{
		NET_RECEIVE_FUNCTOR functor = std::bind(handleRecieve, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
		NET_RECEIVE_FUNCTOR_PTR functorPtr(new NET_RECEIVE_FUNCTOR(functor));

		return AddReceiveCallBack(functorPtr);
	}

	template<typename BaseType>
	bool AddEventCallBack(BaseType* pBase, void (BaseType::*handler)(const NFSOCK, const NF_NET_EVENT, NFINet*))
	{
		NET_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		NET_EVENT_FUNCTOR_PTR functorPtr(new NET_EVENT_FUNCTOR(functor));

		return AddEventCallBack(functorPtr);
	}

	static bool ReceivePB(const int nMsgID, const char * msg, const uint32_t nLen, std::string & strMsg, NFGUID & nPlayer)
	{
		MsgBase xMsg;
		if (!xMsg.ParseFromArray(msg, nLen))
		{
			char szData[MAX_PATH] = { 0 };
			NFSPRINTF(szData, MAX_PATH, "Parse Message Failed from Packet to MsgBase, MessageID: %d\n", nMsgID);
#ifdef DEBUG
			std::cout << "--------------------" << szData << __FUNCTION__ << " " << __LINE__ << std::endl;
#endif // DEBUG

			return false;
		}

		strMsg.assign(xMsg.msg_data().data(), xMsg.msg_data().length());

		nPlayer = PBToNF(xMsg.player_id());

		return true;
	}

	static bool ReceivePB(const int nMsgID, const std::string& strMsgData, google::protobuf::Message & xData, NFGUID & nPlayer)
	{
		return ReceivePB(nMsgID, strMsgData.c_str(), (uint32_t) strMsgData.length(), xData, nPlayer);
	}

	static bool ReceivePB(const int nMsgID, const char * msg, const uint32_t nLen, google::protobuf::Message & xData, NFGUID & nPlayer)
	{
		NFNetProto::MsgBase xMsg;
		if (!xMsg.ParseFromArray(msg, nLen))
		{
			char szData[MAX_PATH] = { 0 };
			NFSPRINTF(szData, MAX_PATH, "Parse Message Failed from Packet to MsgBase, MessageID: %d\n", nMsgID);
#ifdef DEBUG
			std::cout << "--------------------" << szData << __FUNCTION__ << " " << __LINE__ << std::endl;
#endif // DEBUG

			return false;
		}

		if (!xData.ParseFromString(xMsg.msg_data()))
		{
			char szData[MAX_PATH] = { 0 };
			NFSPRINTF(szData, MAX_PATH, "Parse Message Failed from MsgData to ProtocolData, MessageID: %d\n", nMsgID);
#ifdef DEBUG
			std::cout << "--------------------" << szData << __FUNCTION__ << " " << __LINE__ << std::endl;
#endif // DEBUG

			return false;
		}

		nPlayer = PBToNF(xMsg.player_id());

		return true;
	}

	/////////////////
	//as client
	virtual void Initialization(const char* strIP, const unsigned short nPort) = 0;

	//as server
	virtual int Initialization(const unsigned short nPort, const unsigned int nMaxClient) = 0;
	virtual int Initialization(const char* strIP, const unsigned short nPort, const unsigned int nMaxClient) = 0;

	virtual int ExpandBufferSize(const unsigned int size = 1024 * 1024 * 20) = 0;

	virtual void RemoveReceiveCallBack(const int nMsgID) = 0;

	virtual bool AddReceiveCallBack(const int nMsgID, const NET_RECEIVE_FUNCTOR_PTR& cb) = 0;

	virtual bool AddReceiveCallBack(const NET_RECEIVE_FUNCTOR_PTR& cb) = 0;

	virtual bool AddEventCallBack(const NET_EVENT_FUNCTOR_PTR& cb) = 0;

	virtual bool Execute() = 0;


	virtual bool SendMsgWithOutHead(const int nMsgID, const std::string& msg, const NFSOCK nSockIndex) = 0;

	virtual bool SendMsgToAllClientWithOutHead(const int nMsgID, const std::string& msg) = 0;

	virtual bool SendMsgPB(const uint16_t nMsgID, const google::protobuf::Message& xData, const NFSOCK nSockIndex) = 0;
	virtual bool SendMsgPB(const uint16_t nMsgID, const google::protobuf::Message& xData, const NFSOCK nSockIndex, const NFGUID nPlayer) = 0;
	virtual bool SendMsg(const uint16_t nMsgID, const std::string& xData, const NFSOCK nSockIndex) = 0;
	virtual bool SendMsg(const uint16_t nMsgID, const std::string& xData, const NFSOCK nSockIndex, const NFGUID id) = 0;

	virtual bool SendMsgPBToAllClient(const uint16_t nMsgID, const google::protobuf::Message& xData) = 0;

	virtual bool SendMsgPB(const uint16_t nMsgID, const google::protobuf::Message& xData, const NFSOCK nSockIndex, const std::vector<NFGUID>* pClientIDList) = 0;
	virtual bool SendMsgPB(const uint16_t nMsgID, const std::string& strData, const NFSOCK nSockIndex,  const std::vector<NFGUID>* pClientIDList) = 0;

	virtual NFINet* GetNet() = 0;
};

#endif

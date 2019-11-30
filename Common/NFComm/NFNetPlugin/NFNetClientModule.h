#ifndef NF_NET_CLIENT_MODULE_H
#define NF_NET_CLIENT_MODULE_H

#include <iostream>
#include <iosfwd>
#include "NFComm/NFCore/NFQueue.hpp"
#include "NFComm/NFCore/NFConsistentHash.hpp"
#include "NFComm/NFPluginModule/NFINetClientModule.h"
#include "NFComm/NFPluginModule/NFINetModule.h"


class NFNetClientModule : public NFINetClientModule
{
public:

	NFNetClientModule();

    virtual bool Init();

    virtual bool AfterInit();

    virtual bool BeforeShut();

    virtual bool Shut();

    virtual bool Execute();

    virtual void AddServer(const ConnectData& xInfo);

    virtual int ExpandBufferSize(const unsigned int size = 1024 * 1024 * 20);

    virtual int AddReceiveCallBack(const EServerType eType, NET_RECEIVE_FUNCTOR_PTR functorPtr);

    virtual int AddReceiveCallBack(const EServerType eType, const uint16_t nMsgID, NET_RECEIVE_FUNCTOR_PTR functorPtr);

    virtual int AddEventCallBack(const EServerType eType, NET_EVENT_FUNCTOR_PTR functorPtr);
    
    virtual void RemoveReceiveCallBack(const EServerType eType, const uint16_t nMsgID);

    ////////////////////////////////////////////////////////////////////////////////
	virtual void SendByServerIDWithOutHead(const int nServerID, const uint16_t nMsgID, const std::string& strData);

	virtual void SendByServerID(const int nServerID, const uint16_t nMsgID, const std::string& strData);
	virtual void SendByServerID(const int nServerID, const uint16_t nMsgID, const std::string& strData, const NFGUID id);

	virtual void SendToAllServerWithOutHead(const uint16_t nMsgID, const std::string& strData);

	virtual void SendToAllServer(const uint16_t nMsgID, const std::string& strData);
	virtual void SendToAllServer(const uint16_t nMsgID, const std::string& strData, const NFGUID id);

	virtual void SendToAllServerWithOutHead(const EServerType eType, const uint16_t nMsgID, const std::string& strData);

	virtual void SendToAllServer(const EServerType eType, const uint16_t nMsgID, const std::string& strData);
	virtual void SendToAllServer(const EServerType eType, const uint16_t nMsgID, const std::string& strData, const NFGUID id);

	virtual void SendToServerByPB(const int nServerID, const uint16_t nMsgID, const google::protobuf::Message& xData);
	virtual void SendToServerByPB(const int nServerID, const uint16_t nMsgID, const google::protobuf::Message& xData, const NFGUID id);

    virtual void SendToAllServerByPB(const uint16_t nMsgID, const google::protobuf::Message& xData);

    virtual void SendToAllServerByPB(const EServerType eType, const uint16_t nMsgID, const google::protobuf::Message& xData);

    ////////////////////////////////////////////////////////////////////////////////

	virtual void SendBySuitWithOutHead(const EServerType eType, const std::string& strHashKey, const uint16_t nMsgID, const std::string& strData);

	virtual void SendBySuit(const EServerType eType, const std::string& strHashKey, const uint16_t nMsgID, const std::string& strData);
	virtual void SendBySuit(const EServerType eType, const std::string& strHashKey, const uint16_t nMsgID, const std::string& strData, const NFGUID id);

	virtual void SendBySuitWithOutHead(const EServerType eType, const int nHashKey32, const uint16_t nMsgID, const std::string& strData);

	virtual void SendBySuit(const EServerType eType, const int nHashKey32, const uint16_t nMsgID, const std::string& strData);
	virtual void SendBySuit(const EServerType eType, const int nHashKey32, const uint16_t nMsgID, const std::string& strData, const NFGUID id);

	virtual void SendSuitByPB(const EServerType eType, const std::string& strHashKey, const uint16_t nMsgID, const google::protobuf::Message& xData);
	virtual void SendSuitByPB(const EServerType eType, const std::string& strHashKey, const uint16_t nMsgID, const google::protobuf::Message& xData, const NFGUID id);

	virtual void SendSuitByPB(const EServerType eType, const int nHashKey32, const uint16_t nMsgID, const google::protobuf::Message& xData);
	virtual void SendSuitByPB(const EServerType eType, const int nHashKey32, const uint16_t nMsgID, const google::protobuf::Message& xData, const NFGUID id);

    ////////////////////////////////////////////////////////////////////////////////

    virtual NFMapEx<int, ConnectData>& GetServerList();

    virtual NF_SHARE_PTR<ConnectData> GetServerNetInfo(const EServerType eType);

    virtual NF_SHARE_PTR<ConnectData> GetServerNetInfo(const int nServerID);

    virtual NF_SHARE_PTR<ConnectData> GetServerNetInfo(const NFINet* pNet);

	virtual NF_SHARE_PTR<ConnectData> GetServerNetInfo(const NFSOCK nSockIndex);

protected:

    void InitCallBacks(NF_SHARE_PTR<ConnectData> pServerData);

    void ProcessExecute();

private:
    void LogServerInfo();

    void KeepState(NF_SHARE_PTR<ConnectData> pServerData);

    void OnSocketEvent(const NFSOCK fd, const NF_NET_EVENT eEvent, NFINet* pNet);

    int OnConnected(const NFSOCK fd, NFINet* pNet);

    int OnDisConnected(const NFSOCK fd, NFINet* pNet);

    void ProcessAddNetConnect();

protected:
	int64_t mnLastActionTime;
	int64_t mnBufferSize;
    //server_id, server_data
    NFConsistentHashMapEx<int, ConnectData> mxServerMap;
    //server_type, server_id, server_data
    NFMapEx<int, NFConsistentHashMapEx<int, ConnectData>> mxEServerTypeMap;

    std::list<ConnectData> mxTempNetList;

    struct CallBack
    {
        //call back
		//std::map<int, NET_RECEIVE_FUNCTOR_PTR> mxReceiveCallBack;
		std::map<int, std::list<NET_RECEIVE_FUNCTOR_PTR>> mxReceiveCallBack;
        std::list<NET_EVENT_FUNCTOR_PTR> mxEventCallBack;
        std::list<NET_RECEIVE_FUNCTOR_PTR> mxCallBackList;
    };

    NFMapEx<int, CallBack> mxCallBack;
};

#endif

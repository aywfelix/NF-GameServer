#ifndef NF_NET_H
#define NF_NET_H

#include "NFINet.h"
#include <thread>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/thread.h>
#include <event2/event_compat.h>

#pragma pack(push, 1)

class NFNet : public NFINet
{
public:
    NFNet()
    {
        mxBase = NULL;
        listener = NULL;

        mstrIP = "";
        mnPort = 0;
        mbServer = false;
        mbWorking = false;

        mnSendMsgTotal = 0;
        mnReceiveMsgTotal = 0;

        mnBufferSize = 0;
    }

    template <typename BaseType>
    NFNet(BaseType *pBaseType, void (BaseType::*handleRecieve)(const NFSOCK, const int, const char *, const uint32_t), void (BaseType::*handleEvent)(const NFSOCK, const NF_NET_EVENT, NFINet *))
    {
        mxBase = NULL;
        listener = NULL;

        mRecvCB = std::bind(handleRecieve, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        mEventCB = std::bind(handleEvent, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        mstrIP = "";
        mnPort = 0;
        mbServer = false;
        mbWorking = false;

        mnSendMsgTotal = 0;
        mnReceiveMsgTotal = 0;

        mnBufferSize = 0;
    }

    virtual ~NFNet(){};

public:
    virtual bool Execute();

    virtual void Initialization(const char *strIP, const unsigned short nPort);
    virtual int Initialization(const unsigned short nPort, const unsigned int nMaxClient);
    virtual int Initialization(const char *strIP, const unsigned short nPort, const unsigned int nMaxClient);
    virtual int ExpandBufferSize(const unsigned int size);

    virtual bool KickAll();
    virtual bool Final();

    virtual bool SendMsgWithOutHead(const int16_t nMsgID, const char *msg, const size_t nLen, const NFSOCK nSockIndex);

    virtual bool SendMsgWithOutHead(const int16_t nMsgID, const char *msg, const size_t nLen, const std::list<NFSOCK> &fdList);

    virtual bool SendMsgToAllClientWithOutHead(const int16_t nMsgID, const char *msg, const size_t nLen);

    virtual bool CloseNetObject(const NFSOCK nSockIndex);
    virtual bool AddNetObject(const NFSOCK nSockIndex, NetObject *pObject);
    virtual NetObject *GetNetObject(const NFSOCK nSockIndex);

    virtual bool IsServer();
    virtual bool Log(int severity, const char *msg);

	virtual bool SendMsgToAllClient(const char* msg, const size_t nLen);
	virtual bool SendMsg(const char* msg, const size_t nLen, const NFSOCK nSockIndex);
	virtual bool SendMsg(const char* msg, const size_t nLen, const std::list<NFSOCK>& fdList);

private:
    void ExecuteClose();
    bool CloseSocketAll();

    bool Dismantle(NetObject *pObject);

    int InitClientNet();
    int InitServerNet();
    void CloseObject(const NFSOCK nSockIndex);

    static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sa, int socklen, void *user_data);
    static void conn_readcb(struct bufferevent *bev, void *user_data);
    static void conn_writecb(struct bufferevent *bev, void *user_data);
    static void conn_eventcb(struct bufferevent *bev, short events, void *user_data);
    static void log_cb(int severity, const char *msg);
    static void event_fatal_cb(int err);

protected:
    int DeCode(const char *strData, const uint32_t unLen, NFNetProtoHead &xHead);
    int EnCode(const uint16_t unMsgID, const char *strData, const uint32_t unDataLen, std::string &strOutData);

private:
    //<fd,object>

    std::map<NFSOCK, NetObject *> mmObject;
    std::vector<NFSOCK> mvRemoveObject;

    int mnMaxConnect;
    std::string mstrIP;
    int mnPort;
    bool mbServer;

    int mnBufferSize;

    bool mbWorking;
    bool mbTCPStream;

    int64_t mnSendMsgTotal;
    int64_t mnReceiveMsgTotal;

    struct event_base *mxBase;
    struct evconnlistener *listener;
    //////////////////////////////////////////////////////////////////////////

    NET_RECEIVE_FUNCTOR mRecvCB;
    NET_EVENT_FUNCTOR mEventCB;

    //////////////////////////////////////////////////////////////////////////
};

#pragma pack(pop)

#endif

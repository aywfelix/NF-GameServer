#ifndef NF_HTTP_SERVER_H
#define NF_HTTP_SERVER_H

#include "NFIHttpServer.h"
//#include "NFComm/NFCore/NFException.h"
#include "NFComm/NFCore/NFMapEx.hpp"

#if NF_PLATFORM == NF_PLATFORM_WIN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#ifndef S_ISDIR
#define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif
#ifndef LIBEVENT_SRC
#pragma comment( lib, "event.lib")
#endif

#else

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <atomic>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#endif

#include <event2/bufferevent.h>
#include "event2/bufferevent_struct.h"
#include "event2/event.h"
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>


class NFHttpServer : public NFIHttpServer
{
public:
    NFHttpServer()
    {
    }

    template<typename BaseType>
    NFHttpServer(BaseType* pBaseType, bool (BaseType::*handleRecieve)(NF_SHARE_PTR<NFHttpRequest> req), NFWebStatus (BaseType::*handleFilter)(NF_SHARE_PTR<NFHttpRequest> req))
    {
        mxBase = NULL;
		mReceiveCB = std::bind(handleRecieve, pBaseType, std::placeholders::_1);
		mFilter = std::bind(handleFilter, pBaseType, std::placeholders::_1);
    }

    virtual ~NFHttpServer()
	{
		if (mxBase)
		{
			event_base_free(mxBase);
			mxBase = NULL;
		}

        mxHttpRequestMap.ClearAll();
        mxHttpRequestPool.clear();
	};


    virtual bool Execute();

    virtual int InitServer(const unsigned short nPort);

    virtual bool ResponseMsg(NF_SHARE_PTR<NFHttpRequest> req, const std::string& strMsg, NFWebStatus code, const std::string& strReason = "OK");

    virtual NF_SHARE_PTR<NFHttpRequest> GetHttpRequest(const int64_t index);

private:
    static void listener_cb(struct evhttp_request* req, void* arg);

	NF_SHARE_PTR<NFHttpRequest> AllocHttpRequest();

private:
    int64_t mIndex = 0;

    struct event_base* mxBase;
	HTTP_RECEIVE_FUNCTOR mReceiveCB;
	HTTP_FILTER_FUNCTOR mFilter;

	NFMapEx<int64_t, NFHttpRequest> mxHttpRequestMap;
	std::list<NF_SHARE_PTR<NFHttpRequest>> mxHttpRequestPool;
};

#endif

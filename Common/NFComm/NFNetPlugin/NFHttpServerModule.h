#ifndef NF_HTTP_SERVER_MODULE_H
#define NF_HTTP_SERVER_MODULE_H

#include <iostream>
#include "NFComm/NFCore/NFMapEx.hpp"
#include "NFComm/NFCore/NFPerformance.hpp"
#include "NFComm/NFNetPlugin/NFIHttpServer.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"

class NFHttpServerModule
        : public NFIHttpServerModule
{

public:
    NFHttpServerModule();

    virtual ~NFHttpServerModule();

public:

    virtual int InitServer(const unsigned short nPort);

    virtual bool Execute();

    virtual bool ResponseMsg(NF_SHARE_PTR<NFHttpRequest> req, const std::string& strMsg, NFWebStatus code = NFWebStatus::WEB_OK, const std::string& reason = "OK");

private:
	virtual bool OnReceiveNetPack(NF_SHARE_PTR<NFHttpRequest> req);
	virtual NFWebStatus OnFilterPack(NF_SHARE_PTR<NFHttpRequest> req);

	virtual bool AddMsgCB(const std::string& strCommand, const NFHttpType eRequestType, const HTTP_RECEIVE_FUNCTOR_PTR& cb);
	virtual bool AddFilterCB(const std::string& strCommand, const HTTP_FILTER_FUNCTOR_PTR& cb);

private:
	NFIHttpServer* m_pHttpServer;

	NFMapEx<NFHttpType, std::map<std::string, HTTP_RECEIVE_FUNCTOR_PTR>> mMsgCBMap;
	std::map<std::string, HTTP_FILTER_FUNCTOR_PTR> mMsgFliterMap;

    HTTP_RECEIVE_FUNCTOR_PTR mComMsgCBList;
};

#endif

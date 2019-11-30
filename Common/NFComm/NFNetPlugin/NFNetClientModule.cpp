#include "NFNetModule.h"
#include "NFNetClientModule.h"
#include "NFNet.h"
#include "easylogging/easylogging++.h"

NFNetClientModule::NFNetClientModule()
{
    mnBufferSize = 0;
}

bool NFNetClientModule::Init()
{

    for (int i = 0; i < SERVER_TYPE_MAX; ++i)
    {
        NFINetClientModule::AddEventCallBack((EServerType)i, this, &NFNetClientModule::OnSocketEvent);
    }

    return true;
}

bool NFNetClientModule::AfterInit()
{

    return true;
}

bool NFNetClientModule::BeforeShut()
{
    return true;
}

bool NFNetClientModule::Shut()
{
    return true;
}

bool NFNetClientModule::Execute()
{
    ProcessExecute();
    ProcessAddNetConnect();

    return true;
}

void NFNetClientModule::RemoveReceiveCallBack(const EServerType eType, const uint16_t nMsgID)
{
    NF_SHARE_PTR<CallBack> xCallBack = mxCallBack.GetElement(eType);
    if (xCallBack)
    {
        std::map<int, std::list<NET_RECEIVE_FUNCTOR_PTR>>::iterator it = xCallBack->mxReceiveCallBack.find(nMsgID);
        if (xCallBack->mxReceiveCallBack.end() == it)
        {
            xCallBack->mxReceiveCallBack.erase(it);
        }
    }
}

void NFNetClientModule::AddServer(const ConnectData &xInfo)
{
    mxTempNetList.push_back(xInfo);
}

int NFNetClientModule::ExpandBufferSize(const unsigned int size)
{
    if (size > 0)
    {
        mnBufferSize = size;
    }
    return mnBufferSize;
}

int NFNetClientModule::AddReceiveCallBack(const EServerType eType, const uint16_t nMsgID,
                                          NET_RECEIVE_FUNCTOR_PTR functorPtr)
{
    NF_SHARE_PTR<CallBack> xCallBack = mxCallBack.GetElement(eType);
    if (!xCallBack)
    {
        xCallBack = NF_SHARE_PTR<CallBack>(NF_NEW CallBack);
        mxCallBack.AddElement(eType, xCallBack);
    }

    std::list<NET_RECEIVE_FUNCTOR_PTR> xList;
    xList.push_back(functorPtr);

    xCallBack->mxReceiveCallBack.insert(std::map<int, std::list<NET_RECEIVE_FUNCTOR_PTR>>::value_type(nMsgID, xList));

    return 0;
}

int NFNetClientModule::AddReceiveCallBack(const EServerType eType, NET_RECEIVE_FUNCTOR_PTR functorPtr)
{
    NF_SHARE_PTR<CallBack> xCallBack = mxCallBack.GetElement(eType);
    if (!xCallBack)
    {
        xCallBack = NF_SHARE_PTR<CallBack>(NF_NEW CallBack);
        mxCallBack.AddElement(eType, xCallBack);
    }

    xCallBack->mxCallBackList.push_back(functorPtr);

    return 0;
}

int NFNetClientModule::AddEventCallBack(const EServerType eType, NET_EVENT_FUNCTOR_PTR functorPtr)
{
    NF_SHARE_PTR<CallBack> xCallBack = mxCallBack.GetElement(eType);
    if (!xCallBack)
    {
        xCallBack = NF_SHARE_PTR<CallBack>(NF_NEW CallBack);
        mxCallBack.AddElement(eType, xCallBack);
    }

    xCallBack->mxEventCallBack.push_back(functorPtr);
    return 0;
}

void NFNetClientModule::SendByServerIDWithOutHead(const int nServerID, const uint16_t nMsgID, const std::string &strData)
{
    NF_SHARE_PTR<ConnectData> pServer = mxServerMap.GetElement(nServerID);
    if (pServer)
    {
        NF_SHARE_PTR<NFINetModule> pNetModule = pServer->mxNetModule;
        if (pNetModule.get())
        {
            pNetModule->SendMsgWithOutHead(nMsgID, strData, 0);
        }
    }
}

void NFNetClientModule::SendByServerID(const int nServerID, const uint16_t nMsgID, const std::string &strData)
{
    NF_SHARE_PTR<ConnectData> pServer = mxServerMap.GetElement(nServerID);
    if (pServer)
    {
        NF_SHARE_PTR<NFINetModule> pNetModule = pServer->mxNetModule;
        if (pNetModule.get())
        {
            pNetModule->SendMsg(nMsgID, strData, 0);
        }
    }
}

void NFNetClientModule::SendByServerID(const int nServerID, const uint16_t nMsgID, const std::string &strData, const NFGUID id)
{
    NF_SHARE_PTR<ConnectData> pServer = mxServerMap.GetElement(nServerID);
    if (pServer)
    {
        NF_SHARE_PTR<NFINetModule> pNetModule = pServer->mxNetModule;
        if (pNetModule.get())
        {
            pNetModule->SendMsg(nMsgID, strData, 0, id);
        }
    }
}

void NFNetClientModule::SendToAllServerWithOutHead(const uint16_t nMsgID, const std::string &strData)
{
    NF_SHARE_PTR<ConnectData> pServer = mxServerMap.First();
    while (pServer)
    {
        NF_SHARE_PTR<NFINetModule> pNetModule = pServer->mxNetModule;
        if (pNetModule)
        {
            pNetModule->SendMsgWithOutHead(nMsgID, strData, 0);
        }
        pServer = mxServerMap.Next();
    }
}

void NFNetClientModule::SendToAllServer(const uint16_t nMsgID, const std::string &strData)
{
    NF_SHARE_PTR<ConnectData> pServer = mxServerMap.First();
    while (pServer)
    {
        NF_SHARE_PTR<NFINetModule> pNetModule = pServer->mxNetModule;
        if (pNetModule)
        {
            pNetModule->SendMsg(nMsgID, strData, 0);
        }

        pServer = mxServerMap.Next();
    }
}

void NFNetClientModule::SendToAllServer(const uint16_t nMsgID, const std::string &strData, const NFGUID id)
{
    NF_SHARE_PTR<ConnectData> pServer = mxServerMap.First();
    while (pServer)
    {
        NF_SHARE_PTR<NFINetModule> pNetModule = pServer->mxNetModule;
        if (pNetModule)
        {
            pNetModule->SendMsg(nMsgID, strData, 0, id);
        }

        pServer = mxServerMap.Next();
    }
}

void NFNetClientModule::SendToAllServerWithOutHead(const EServerType eType, const uint16_t nMsgID, const std::string &strData)
{
    NF_SHARE_PTR<ConnectData> pServer = mxServerMap.First();
    while (pServer)
    {
        NF_SHARE_PTR<NFINetModule> pNetModule = pServer->mxNetModule;
        if (pNetModule && eType == pServer->eEServerType)
        {
            pNetModule->SendMsgWithOutHead(nMsgID, strData, 0);
        }

        pServer = mxServerMap.Next();
    }
}

void NFNetClientModule::SendToAllServer(const EServerType eType, const uint16_t nMsgID, const std::string &strData)
{
    NF_SHARE_PTR<ConnectData> pServer = mxServerMap.First();
    while (pServer)
    {
        NF_SHARE_PTR<NFINetModule> pNetModule = pServer->mxNetModule;
        if (pNetModule && eType == pServer->eEServerType)
        {
            pNetModule->SendMsg(nMsgID, strData, 0);
        }

        pServer = mxServerMap.Next();
    }
}

void NFNetClientModule::SendToAllServer(const EServerType eType, const uint16_t nMsgID, const std::string &strData, const NFGUID id)
{
    NF_SHARE_PTR<ConnectData> pServer = mxServerMap.First();
    while (pServer)
    {
        NF_SHARE_PTR<NFINetModule> pNetModule = pServer->mxNetModule;
        if (pNetModule && eType == pServer->eEServerType)
        {
            pNetModule->SendMsg(nMsgID, strData, 0, id);
        }

        pServer = mxServerMap.Next();
    }
}

void NFNetClientModule::SendToServerByPB(const int nServerID, const uint16_t nMsgID, const google::protobuf::Message &xData)
{
    NF_SHARE_PTR<ConnectData> pServer = mxServerMap.GetElement(nServerID);
    if (pServer)
    {
        NF_SHARE_PTR<NFINetModule> pNetModule = pServer->mxNetModule;
        if (pNetModule)
        {
            pNetModule->SendMsgPB(nMsgID, xData, 0);
        }
    }
}

void NFNetClientModule::SendToServerByPB(const int nServerID, const uint16_t nMsgID, const google::protobuf::Message &xData, const NFGUID id)
{
    NF_SHARE_PTR<ConnectData> pServer = mxServerMap.GetElement(nServerID);
    if (pServer)
    {
        NF_SHARE_PTR<NFINetModule> pNetModule = pServer->mxNetModule;
        if (pNetModule)
        {
            pNetModule->SendMsgPB(nMsgID, xData, 0, id);
        }
    }
}

void NFNetClientModule::SendToAllServerByPB(const uint16_t nMsgID, const google::protobuf::Message &xData)
{
    NF_SHARE_PTR<ConnectData> pServer = mxServerMap.First();
    while (pServer)
    {
        NF_SHARE_PTR<NFINetModule> pNetModule = pServer->mxNetModule;
        if (pNetModule)
        {
            pNetModule->SendMsgPB(nMsgID, xData, 0);
        }

        pServer = mxServerMap.Next();
    }
}

void NFNetClientModule::SendToAllServerByPB(const EServerType eType, const uint16_t nMsgID,
                                            const google::protobuf::Message &xData)
{
    NF_SHARE_PTR<ConnectData> pServer = mxServerMap.First();
    while (pServer)
    {
        NF_SHARE_PTR<NFINetModule> pNetModule = pServer->mxNetModule;
        if (pNetModule && eType == pServer->eEServerType && pServer->eState == ConnectDataState::NORMAL)
        {
            pNetModule->SendMsgPB(nMsgID, xData, 0);
        }

        pServer = mxServerMap.Next();
    }
}

void NFNetClientModule::SendBySuitWithOutHead(const EServerType eType, const std::string &strHashKey, const uint16_t nMsgID, const std::string &strData)
{
    uint32_t nCRC32 = NFrame::CRC32(strHashKey);
    SendBySuitWithOutHead(eType, nCRC32, nMsgID, strData);
}

void NFNetClientModule::SendBySuit(const EServerType eType, const std::string &strHashKey, const uint16_t nMsgID,
                                   const std::string &strData)
{
    uint32_t nCRC32 = NFrame::CRC32(strHashKey);
    SendBySuit(eType, nCRC32, nMsgID, strData);
}

void NFNetClientModule::SendBySuit(const EServerType eType, const std::string &strHashKey, const uint16_t nMsgID, const std::string &strData, const NFGUID id)
{
    uint32_t nCRC32 = NFrame::CRC32(strHashKey);
    SendBySuit(eType, nCRC32, nMsgID, strData, id);
}

void NFNetClientModule::SendBySuitWithOutHead(const EServerType eType, const int nHashKey32, const uint16_t nMsgID, const std::string &strData)
{
    NF_SHARE_PTR<NFConsistentHashMapEx<int, ConnectData>> xConnectDataMap = mxEServerTypeMap.GetElement(eType);
    if (xConnectDataMap)
    {
        NF_SHARE_PTR<ConnectData> pConnectData = xConnectDataMap->GetElementBySuit(nHashKey32);
        if (pConnectData)
        {
            SendByServerIDWithOutHead(pConnectData->nGameID, nMsgID, strData);
        }
    }
}

void NFNetClientModule::SendBySuit(const EServerType eType, const int nHashKey, const uint16_t nMsgID, const std::string &strData)
{
    NF_SHARE_PTR<NFConsistentHashMapEx<int, ConnectData>> xConnectDataMap = mxEServerTypeMap.GetElement(eType);
    if (xConnectDataMap)
    {
        NF_SHARE_PTR<ConnectData> pConnectData = xConnectDataMap->GetElementBySuit(nHashKey);
        if (pConnectData)
        {
            SendByServerID(pConnectData->nGameID, nMsgID, strData);
        }
    }
}

void NFNetClientModule::SendBySuit(const EServerType eType, const int nHashKey32, const uint16_t nMsgID, const std::string &strData, const NFGUID id)
{
    NF_SHARE_PTR<NFConsistentHashMapEx<int, ConnectData>> xConnectDataMap = mxEServerTypeMap.GetElement(eType);
    if (xConnectDataMap)
    {
        NF_SHARE_PTR<ConnectData> pConnectData = xConnectDataMap->GetElementBySuit(nHashKey32);
        if (pConnectData)
        {
            SendByServerID(pConnectData->nGameID, nMsgID, strData, id);
        }
    }
}

void NFNetClientModule::SendSuitByPB(const EServerType eType, const std::string &strHashKey, const uint16_t nMsgID, const google::protobuf::Message &xData)
{
    uint32_t nCRC32 = NFrame::CRC32(strHashKey);
    SendSuitByPB(eType, nCRC32, nMsgID, xData);
}

void NFNetClientModule::SendSuitByPB(const EServerType eType, const std::string &strHashKey, const uint16_t nMsgID, const google::protobuf::Message &xData, const NFGUID id)
{
    uint32_t nCRC32 = NFrame::CRC32(strHashKey);
    SendSuitByPB(eType, nCRC32, nMsgID, xData, id);
}

void NFNetClientModule::SendSuitByPB(const EServerType eType, const int nHashKey, const uint16_t nMsgID, const google::protobuf::Message &xData)
{
    NF_SHARE_PTR<NFConsistentHashMapEx<int, ConnectData>> xConnectDataMap = mxEServerTypeMap.GetElement(eType);
    if (xConnectDataMap)
    {
        NF_SHARE_PTR<ConnectData> pConnectData = xConnectDataMap->GetElementBySuit(nHashKey);
        if (pConnectData)
        {
            SendToServerByPB(pConnectData->nGameID, nMsgID, xData);
        }
    }
}

void NFNetClientModule::SendSuitByPB(const EServerType eType, const int nHashKey32, const uint16_t nMsgID, const google::protobuf::Message &xData, const NFGUID id)
{
    NF_SHARE_PTR<NFConsistentHashMapEx<int, ConnectData>> xConnectDataMap = mxEServerTypeMap.GetElement(eType);
    if (xConnectDataMap)
    {
        NF_SHARE_PTR<ConnectData> pConnectData = xConnectDataMap->GetElementBySuit(nHashKey32);
        if (pConnectData)
        {
            SendToServerByPB(pConnectData->nGameID, nMsgID, xData, id);
        }
    }
}

NF_SHARE_PTR<ConnectData> NFNetClientModule::GetServerNetInfo(const EServerType eType)
{
    NF_SHARE_PTR<NFConsistentHashMapEx<int, ConnectData>> xConnectDataMap = mxEServerTypeMap.GetElement(eType);
    if (xConnectDataMap)
    {
        return xConnectDataMap->GetElementBySuitRandom();
    }

    return nullptr;
}

NF_SHARE_PTR<ConnectData> NFNetClientModule::GetServerNetInfo(const int nServerID)
{
    return mxServerMap.GetElement(nServerID);
}

NFMapEx<int, ConnectData> &NFNetClientModule::GetServerList()
{
    return mxServerMap;
}

NF_SHARE_PTR<ConnectData> NFNetClientModule::GetServerNetInfo(const NFINet *pNet)
{
    int nServerID = 0;
    for (NF_SHARE_PTR<ConnectData> pData = mxServerMap.First(nServerID);
         pData != NULL; pData = mxServerMap.Next(nServerID))
    {
        if (pData->mxNetModule && pNet == pData->mxNetModule->GetNet())
        {
            return pData;
        }
    }

    return NF_SHARE_PTR<ConnectData>(NULL);
}

NF_SHARE_PTR<ConnectData> NFNetClientModule::GetServerNetInfo(const NFSOCK nSockIndex)
{
	int nServerID = 0;
	for (NF_SHARE_PTR<ConnectData> pData = mxServerMap.First(nServerID);
		pData != NULL; pData = mxServerMap.Next(nServerID))
	{
		if (pData->mxNetModule )
		{
			NFINet* pNet = pData->mxNetModule->GetNet();
			if (pNet && pNet->GetNetObject(nSockIndex))
			{
				return pData;
			}
		}
	}

	return NF_SHARE_PTR<ConnectData>(NULL);
}

void NFNetClientModule::InitCallBacks(NF_SHARE_PTR<ConnectData> pServerData)
{
    NF_SHARE_PTR<CallBack> xCallBack = mxCallBack.GetElement(pServerData->eEServerType);
    if (!xCallBack)
    {
        xCallBack = NF_SHARE_PTR<CallBack>(NF_NEW CallBack);
        mxCallBack.AddElement(pServerData->eEServerType, xCallBack);
    }

    //add msg callback
    std::map<int, std::list<NET_RECEIVE_FUNCTOR_PTR>>::iterator itReciveCB = xCallBack->mxReceiveCallBack.begin();
    for (; xCallBack->mxReceiveCallBack.end() != itReciveCB; ++itReciveCB)
    {
        std::list<NET_RECEIVE_FUNCTOR_PTR> &xList = itReciveCB->second;
        for (std::list<NET_RECEIVE_FUNCTOR_PTR>::iterator itList = xList.begin(); itList != xList.end(); ++itList)
        {
            pServerData->mxNetModule->AddReceiveCallBack(itReciveCB->first, *itList);
        }
    }

    //add event callback
    std::list<NET_EVENT_FUNCTOR_PTR>::iterator itEventCB = xCallBack->mxEventCallBack.begin();
    for (; xCallBack->mxEventCallBack.end() != itEventCB; ++itEventCB)
    {
        pServerData->mxNetModule->AddEventCallBack(*itEventCB);
    }

    std::list<NET_RECEIVE_FUNCTOR_PTR>::iterator itCB = xCallBack->mxCallBackList.begin();
    for (; xCallBack->mxCallBackList.end() != itCB; ++itCB)
    {
        pServerData->mxNetModule->AddReceiveCallBack(*itCB);
    }
}

void NFNetClientModule::ProcessExecute()
{
    NF_SHARE_PTR<ConnectData> pServerData = mxServerMap.First();
    while (pServerData)
    {
        switch (pServerData->eState)
        {
        case ConnectDataState::DISCONNECT:
        {
            if (NULL != pServerData->mxNetModule)
            {
                pServerData->mxNetModule = nullptr;
                pServerData->eState = ConnectDataState::RECONNECT;
            }
        }
        break;
        case ConnectDataState::CONNECTING:
        {
            if (pServerData->mxNetModule)
            {
                pServerData->mxNetModule->Execute();
            }
        }
        break;
        case ConnectDataState::NORMAL:
        {
            if (pServerData->mxNetModule)
            {
                pServerData->mxNetModule->Execute();

                KeepState(pServerData);
            }
        }
        break;
        case ConnectDataState::RECONNECT:
        {
            if ((pServerData->mnLastActionTime + 10) >= time(0))
            {
                break;
            }

            if (nullptr != pServerData->mxNetModule)
            {
                pServerData->mxNetModule = nullptr;
            }

            pServerData->eState = ConnectDataState::CONNECTING;
            pServerData->mxNetModule = NF_SHARE_PTR<NFINetModule>(NF_NEW NFNetModule());

            pServerData->mxNetModule->Initialization(pServerData->strIP.c_str(), pServerData->nPort);

            InitCallBacks(pServerData);
        }
        break;
        default:
            break;
        }

        pServerData = mxServerMap.Next();
    }
}

void NFNetClientModule::LogServerInfo()
{
    //ILOG("This is a client, begin to print Server Info----------------------------------");

    ConnectData *pServerData = mxServerMap.FirstNude();
    while (nullptr != pServerData)
    {
        std::ostringstream stream;
        stream << "Type: " << pServerData->eEServerType << " Server ID: " << pServerData->nGameID << " State: "
               << pServerData->eState << " IP: " << pServerData->strIP << " Port: " << pServerData->nPort;

        pServerData = mxServerMap.NextNude();
    }
    // ILOG("This is a client, end to print Server Info----------------------------------");
}

void NFNetClientModule::KeepState(NF_SHARE_PTR<ConnectData> pServerData)
{
    if (pServerData->mnLastActionTime + 10 > time(NULL))
    {
        return;
    }

    pServerData->mnLastActionTime = time(NULL);

    // KeepReport(pServerData);
    LogServerInfo();
}

void NFNetClientModule::OnSocketEvent(const NFSOCK fd, const NF_NET_EVENT eEvent, NFINet *pNet)
{
    if (eEvent & NF_NET_EVENT::NF_NET_EVENT_CONNECTED)
    {
        OnConnected(fd, pNet);
    }
    else
    {
        OnDisConnected(fd, pNet);
    }
}

int NFNetClientModule::OnConnected(const NFSOCK fd, NFINet *pNet)
{
    NF_SHARE_PTR<ConnectData> pServerInfo = GetServerNetInfo(pNet);
    if (pServerInfo)
    {
        /////////////////////////////////////////////////////////////////////////////////////
        //AddServerWeightData(pServerInfo);
        pServerInfo->eState = ConnectDataState::NORMAL;

        //for type--suit
        NF_SHARE_PTR<NFConsistentHashMapEx<int, ConnectData>> xConnectDataMap = mxEServerTypeMap.GetElement(pServerInfo->eEServerType);
        if (!xConnectDataMap)
        {
            xConnectDataMap = NF_SHARE_PTR<NFConsistentHashMapEx<int, ConnectData>>(NF_NEW NFConsistentHashMapEx<int, ConnectData>());
            mxEServerTypeMap.AddElement(pServerInfo->eEServerType, xConnectDataMap);
        }

        xConnectDataMap->AddElement(pServerInfo->nGameID, pServerInfo);
    }

    return 0;
}

int NFNetClientModule::OnDisConnected(const NFSOCK fd, NFINet *pNet)
{
    NF_SHARE_PTR<ConnectData> pServerInfo = GetServerNetInfo(pNet);
    if (nullptr != pServerInfo)
    {
        /////////////////////////////////////////////////////////////////////////////////////
        //RemoveServerWeightData(pServerInfo);
        pServerInfo->eState = ConnectDataState::DISCONNECT;
        pServerInfo->mnLastActionTime = time(0);

        //for type--suit
        NF_SHARE_PTR<NFConsistentHashMapEx<int, ConnectData>> xConnectDataMap = mxEServerTypeMap.GetElement(pServerInfo->eEServerType);
        if (xConnectDataMap)
        {
            xConnectDataMap->RemoveElement(pServerInfo->nGameID);
        }
    }

    return 0;
}

void NFNetClientModule::ProcessAddNetConnect()
{
    std::list<ConnectData>::iterator it = mxTempNetList.begin();
    for (; it != mxTempNetList.end(); ++it)
    {
        const ConnectData &xInfo = *it;
        NF_SHARE_PTR<ConnectData> xServerData = mxServerMap.GetElement(xInfo.nGameID);
        if (nullptr == xServerData)
        {
            xServerData = NF_SHARE_PTR<ConnectData>(NF_NEW ConnectData());

            xServerData->nGameID = xInfo.nGameID;
            xServerData->eEServerType = xInfo.eEServerType;
            xServerData->strIP = xInfo.strIP;
            xServerData->strName = xInfo.strName;
            xServerData->eState = ConnectDataState::CONNECTING;
            xServerData->nPort = xInfo.nPort;
            xServerData->mnLastActionTime = time(0);

            xServerData->mxNetModule = NF_SHARE_PTR<NFINetModule>(NF_NEW NFNetModule());

            xServerData->mxNetModule->Initialization(xServerData->strIP.c_str(), xServerData->nPort);
			LOG(INFO) << " connnect to " << xServerData->strName << " " << xServerData->nPort;
            xServerData->mxNetModule->ExpandBufferSize(mnBufferSize);

            InitCallBacks(xServerData);

            mxServerMap.AddElement(xInfo.nGameID, xServerData);
        }
        else
        {
            xServerData->nWorkLoad = xInfo.nWorkLoad;
        }
    }

    mxTempNetList.clear();
}
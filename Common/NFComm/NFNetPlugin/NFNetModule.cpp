#include "NFNetModule.h"

NFNetModule::NFNetModule()
{
    mnBufferSize = 0;
    m_pNet = NULL;
}

NFNetModule::~NFNetModule()
{
    if (m_pNet)
    {
        m_pNet->Final();
    }

    delete m_pNet;
    m_pNet = NULL;
}

bool NFNetModule::Init()
{

    return true;
}

bool NFNetModule::AfterInit()
{
    return true;
}

void NFNetModule::Initialization(const char *strIP, const unsigned short nPort)
{
    m_pNet = NF_NEW NFNet(this, &NFNetModule::OnReceiveNetPack, &NFNetModule::OnSocketNetEvent);
    m_pNet->ExpandBufferSize(mnBufferSize);
    m_pNet->Initialization(strIP, nPort);
}

int NFNetModule::Initialization(const unsigned short nPort, const unsigned int nMaxClient)
{
    m_pNet = NF_NEW NFNet(this, &NFNetModule::OnReceiveNetPack, &NFNetModule::OnSocketNetEvent);
    m_pNet->ExpandBufferSize(mnBufferSize);
    return m_pNet->Initialization(nMaxClient, nPort);
}

int NFNetModule::Initialization(const char *strIP, const unsigned short nPort, const unsigned int nMaxClient)
{
    m_pNet = NF_NEW NFNet(this, &NFNetModule::OnReceiveNetPack, &NFNetModule::OnSocketNetEvent);
    m_pNet->ExpandBufferSize(mnBufferSize);
    return m_pNet->Initialization(strIP, nPort, nMaxClient);
}

int NFNetModule::ExpandBufferSize(const unsigned int size)
{
    if (size > 0)
    {
        mnBufferSize = size;
        if (m_pNet)
        {
            m_pNet->ExpandBufferSize(mnBufferSize);
        }
    }

    return mnBufferSize;
}

void NFNetModule::RemoveReceiveCallBack(const int nMsgID)
{
    std::map<int, std::list<NET_RECEIVE_FUNCTOR_PTR>>::iterator it = mxReceiveCallBack.find(nMsgID);
    if (mxReceiveCallBack.end() != it)
    {
        mxReceiveCallBack.erase(it);
    }
}

bool NFNetModule::AddReceiveCallBack(const int nMsgID, const NET_RECEIVE_FUNCTOR_PTR &cb)
{
    if (mxReceiveCallBack.find(nMsgID) == mxReceiveCallBack.end())
    {
        std::list<NET_RECEIVE_FUNCTOR_PTR> xList;
        xList.push_back(cb);
        mxReceiveCallBack.insert(std::map<int, std::list<NET_RECEIVE_FUNCTOR_PTR>>::value_type(nMsgID, xList));
        return true;
    }

    std::map<int, std::list<NET_RECEIVE_FUNCTOR_PTR>>::iterator it = mxReceiveCallBack.find(nMsgID);
    it->second.push_back(cb);

    return true;
}

bool NFNetModule::AddReceiveCallBack(const NET_RECEIVE_FUNCTOR_PTR &cb)
{
    mxCallBackList.push_back(cb);

    return true;
}

bool NFNetModule::AddEventCallBack(const NET_EVENT_FUNCTOR_PTR &cb)
{
    mxEventCallBackList.push_back(cb);

    return true;
}

bool NFNetModule::Execute()
{
    if (!m_pNet)
    {
        return false;
    }

    KeepAlive();

    return m_pNet->Execute();
}

bool NFNetModule::SendMsgWithOutHead(const int nMsgID, const std::string &msg, const NFSOCK nSockIndex)
{
    bool bRet = m_pNet->SendMsgWithOutHead(nMsgID, msg.c_str(), (uint32_t)msg.length(), nSockIndex);
    return bRet;
}

bool NFNetModule::SendMsgToAllClientWithOutHead(const int nMsgID, const std::string &msg)
{
    bool bRet = m_pNet->SendMsgToAllClientWithOutHead(nMsgID, msg.c_str(), (uint32_t)msg.length());
    return bRet;
}

bool NFNetModule::SendMsgPB(const uint16_t nMsgID, const google::protobuf::Message &xData, const NFSOCK nSockIndex, const NFGUID id)
{
    NFNetProto::MsgBase xMsg;
    if (!xData.SerializeToString(xMsg.mutable_msg_data()))
    {
        std::ostringstream stream;
        stream << " SendMsgPB Message to  " << nSockIndex;
        stream << " Failed For Serialize of MsgData, MessageID " << nMsgID;

        return false;
    }

    NFNetProto::Ident *pPlayerID = xMsg.mutable_player_id();
    *pPlayerID = NFToPB(id);

    std::string strMsg;
    if (!xMsg.SerializeToString(&strMsg))
    {
        std::ostringstream stream;
        stream << " SendMsgPB Message to  " << nSockIndex;
        stream << " Failed For Serialize of MsgBase, MessageID " << nMsgID;

        return false;
    }

    return SendMsgWithOutHead(nMsgID, strMsg, nSockIndex);
}

bool NFNetModule::SendMsg(const uint16_t nMsgID, const std::string &xData, const NFSOCK nSockIndex)
{
    return SendMsgWithOutHead(nMsgID, xData, nSockIndex);
}

bool NFNetModule::SendMsg(const uint16_t nMsgID, const std::string &xData, const NFSOCK nSockIndex, const NFGUID id)
{
    NFNetProto::MsgBase xMsg;
    xMsg.set_msg_data(xData.data(), xData.length());

    NFNetProto::Ident *pPlayerID = xMsg.mutable_player_id();
    *pPlayerID = NFToPB(id);

    std::string strMsg;
    if (!xMsg.SerializeToString(&strMsg))
    {
        std::ostringstream stream;
        stream << " SendMsgPB Message to  " << nSockIndex;
        stream << " Failed For Serialize of MsgBase, MessageID " << nMsgID;

        return false;
    }

    return SendMsgWithOutHead(nMsgID, strMsg, nSockIndex);
}

bool NFNetModule::SendMsgPB(const uint16_t nMsgID, const google::protobuf::Message &xData, const NFSOCK nSockIndex)
{
    NFNetProto::MsgBase xMsg;
    if (!xData.SerializeToString(xMsg.mutable_msg_data()))
    {
        return false;
    }

    NFNetProto::Ident *pPlayerID = xMsg.mutable_player_id();
    *pPlayerID = NFToPB(NFGUID());

    std::string strMsg;
    if (!xMsg.SerializeToString(&strMsg))
    {
        return false;
    }

    SendMsgWithOutHead(nMsgID, strMsg, nSockIndex);

    return true;
}

bool NFNetModule::SendMsgPBToAllClient(const uint16_t nMsgID, const google::protobuf::Message &xData)
{
    NFNetProto::MsgBase xMsg;
    if (!xData.SerializeToString(xMsg.mutable_msg_data()))
    {
        return false;
    }

    std::string strMsg;
    if (!xMsg.SerializeToString(&strMsg))
    {
        return false;
    }

    return SendMsgToAllClientWithOutHead(nMsgID, strMsg);
}

bool NFNetModule::SendMsgPB(const uint16_t nMsgID, const google::protobuf::Message &xData, const NFSOCK nSockIndex, const std::vector<NFGUID> *pClientIDList)
{
    if (!m_pNet)
    {
        return false;
    }

    NFNetProto::MsgBase xMsg;
    if (!xData.SerializeToString(xMsg.mutable_msg_data()))
    {
        return false;
    }

    NFNetProto::Ident *pPlayerID = xMsg.mutable_player_id();
    *pPlayerID = NFToPB(NFGUID());
    if (pClientIDList)
    {
        for (int i = 0; i < pClientIDList->size(); ++i)
        {
            const NFGUID &ClientID = (*pClientIDList)[i];

            NFNetProto::Ident *pData = xMsg.add_player_client_list();
            if (pData)
            {
                *pData = NFToPB(ClientID);
            }
        }
    }

    std::string strMsg;
    if (!xMsg.SerializeToString(&strMsg))
    {
        return false;
    }

    return SendMsgWithOutHead(nMsgID, strMsg, nSockIndex);
}

bool NFNetModule::SendMsgPB(const uint16_t nMsgID, const std::string &strData, const NFSOCK nSockIndex, const std::vector<NFGUID> *pClientIDList)
{
    if (!m_pNet)
    {
        return false;
    }

    NFNetProto::MsgBase xMsg;
    xMsg.set_msg_data(strData.data(), strData.length());

    NFNetProto::Ident *pPlayerID = xMsg.mutable_player_id();
    *pPlayerID = NFToPB(NFGUID());
    if (pClientIDList)
    {
        for (int i = 0; i < pClientIDList->size(); ++i)
        {
            const NFGUID &ClientID = (*pClientIDList)[i];

            NFNetProto::Ident *pData = xMsg.add_player_client_list();
            if (pData)
            {
                *pData = NFToPB(ClientID);
            }
        }
    }

    std::string strMsg;
    if (!xMsg.SerializeToString(&strMsg))
    {
        return false;
    }

    return SendMsgWithOutHead(nMsgID, strMsg, nSockIndex);
}

NFINet *NFNetModule::GetNet()
{
    return m_pNet;
}

void NFNetModule::OnReceiveNetPack(const NFSOCK nSockIndex, const int nMsgID, const char *msg, const uint32_t nLen)
{

    NFPerformance performance;

    std::map<int, std::list<NET_RECEIVE_FUNCTOR_PTR>>::iterator it = mxReceiveCallBack.find(nMsgID);
    if (mxReceiveCallBack.end() != it)
    {
        std::list<NET_RECEIVE_FUNCTOR_PTR> &xFunList = it->second;
        for (std::list<NET_RECEIVE_FUNCTOR_PTR>::iterator itList = xFunList.begin(); itList != xFunList.end(); ++itList)
        {
            NET_RECEIVE_FUNCTOR_PTR &pFunPtr = *itList;
            NET_RECEIVE_FUNCTOR *pFunc = pFunPtr.get();
            pFunc->operator()(nSockIndex, nMsgID, msg, nLen);
        }
    }
    else
    {
        for (std::list<NET_RECEIVE_FUNCTOR_PTR>::iterator itList = mxCallBackList.begin(); itList != mxCallBackList.end(); ++itList)
        {
            NET_RECEIVE_FUNCTOR_PTR &pFunPtr = *itList;
            NET_RECEIVE_FUNCTOR *pFunc = pFunPtr.get();
            pFunc->operator()(nSockIndex, nMsgID, msg, nLen);
        }
    }

    if (performance.CheckTimePoint(1))
    {
        std::ostringstream os;
        os << "---------------net module performance problem------------------- ";
        os << performance.TimeScope();
        os << "---------- MsgID: ";
        os << nMsgID;
    }
}

void NFNetModule::OnSocketNetEvent(const NFSOCK nSockIndex, const NF_NET_EVENT eEvent, NFINet *pNet)
{
    for (std::list<NET_EVENT_FUNCTOR_PTR>::iterator it = mxEventCallBackList.begin();
         it != mxEventCallBackList.end(); ++it)
    {
        NET_EVENT_FUNCTOR_PTR &pFunPtr = *it;
        NET_EVENT_FUNCTOR *pFunc = pFunPtr.get();
        pFunc->operator()(nSockIndex, eEvent, pNet);
    }
}

void NFNetModule::KeepAlive()
{
    if (!m_pNet)
    {
        return;
    }

    if (m_pNet->IsServer())
    {
        return;
    }

    if (nLastTime + 10 > time(0))
    {
        return;
    }

    nLastTime = time(0);

    // NFNetProto::ServerHeartBeat xMsg;
    // xMsg.set_count(0);

    /*SendMsgPB(NFNetProto::EGameMsgID::EGMI_STS_HEART_BEAT, xMsg, 0);*/
}
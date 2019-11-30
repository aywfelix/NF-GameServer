#pragma once

#include "NFComm/NFNetPlugin/NetServerBase.h"

class MasterNodeServer : public NetServerBase{
public:
    bool InitHelper();
    void AfterReportToServer(NF_SHARE_PTR<ServerData> pReportServerData);
    void SyncNodeDataToAll(EServerType nType);
};
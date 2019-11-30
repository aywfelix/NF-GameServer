#include "ClientModule/ModuleChat.h"
#include "Packet/Player.h"
#include "easylogging/easylogging++.h"

int ModuleChat::ChatReq(Player* pPlayer, Packet* pPacket)
{
	if (!pPlayer && !pPacket)
	{
		return -1;
	}
	
	auto pMsg = (Chat_ChatReq*)pPacket->pMsg;
	const std::string msg = pMsg->chat_msg();

	LOG(INFO) << "recv msg from gate server " << msg;
	
	Chat_ChatReply reply;
	reply.set_ret(0);
	reply.set_chat_msg(msg);

	pPlayer->SendMsg(pPacket->msg_id, &reply);
	return 0;
}
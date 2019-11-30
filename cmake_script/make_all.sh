#!/bin

#先编译依赖库
sh./make_protobuf.sh
sh ./make_common.sh
sh./make_navmesh.sh

#编译各个服务器
sh ./make_chatserver.sh
sh./make_gameserver.sh
sh ./make_dbserver.sh
sh./make_worldserver.sh
sh ./make_loginserver.sh
sh./make_gateserver.sh

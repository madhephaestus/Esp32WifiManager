
#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <SimplePacketComs.h>
#include <server/UDPSimplePacket.h>

//#include <controller/ControlFactory.h>
void launchControllerServer() ;

void addServer(PacketEventAbstract * eventImplementation );
void setNameUdpDevice(String *robot );
void launchControllerReciver(const char * ssid, const char * pwd,String * controllerName);
void loopServer();
void loopReciver();



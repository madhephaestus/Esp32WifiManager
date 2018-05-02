
#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <SimplePacketComs.h>
#include <WiiChuck.h>
#include <server/UDPSimplePacket.h>
#include <server/WiiClassicServer.h>
//#include <server/WiiChuckServer.h>
#include <controller/UdpController.h>
#include "controller/AbstractController.h"

//#include <controller/ControlFactory.h>
void launchControllerServer() ;

void addServer(PacketEventAbstract * eventImplementation );
void setNameUdpDevice(String *robot );
void launchControllerReciver(const char * ssid, const char * pwd,String * controllerName);
void loopServer();
void loopReciver();

AbstractController * getController(int id);

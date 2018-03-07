
#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <SimplePacketComs.h>
#include <WiiChuck.h>
#include <server/UDPSimplePacket.h>
#include <server/WiiClassicServer.h>
#include <controller/UdpController.h>
#include "controller/AbstractController.h"

//#include <controller/ControlFactory.h>
//void launchControllerServer(const char * ssid, const char * pwd,Classic * classic);
void launchControllerReciver(const char * ssid, const char * pwd);
//void loopServer();
void loopReciver();

AbstractController * getUdpController(int id);

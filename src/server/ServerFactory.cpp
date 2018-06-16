/*
 * ControllerFactory.c
 *
 *  Created on: Mar 6, 2018
 *      Author: hephaestus
 */

#include <Esp32SimplePacketComs.h>
#include <SimplePacketComs.h>
#include <server/UDPSimplePacket.h>
#include "NameCheckerServer.h"
#include <wifi/WifiManager.h>

//Are we currently connected?
//The udp library class
static UDPSimplePacket * simple = NULL;

static WifiManager manager;

void launchControllerServer() {
	if (simple == NULL) {
		// put your setup code here: to run once:
		manager.setup();

		simple = new UDPSimplePacket();
		//Connect to the WiFi network
		//delay(1000);

	}
}
void addServer(PacketEventAbstract * eventImplementation) {
	if (simple != NULL)
		simple->attach(eventImplementation);
}

void setNameUdpDevice(String *robot) {
	PacketEventAbstract * ptr= new NameCheckerServer(robot);
	addServer(ptr);
	Serial.println("Setting controller name to: " + robot[0]);
}

void loopServer() {
	manager.loop();
	if (manager.getState() == Connected && simple != NULL) {

		simple->server();
	}
}



/*
 * UdpNameSearch.cpp
 *
 *  Created on: Mar 6, 2018
 *      Author: hephaestus
 */

#include <client/AbstractPacketType.h>
#include <device/UdpNameSearch.h>

UdpNameSearch::UdpNameSearch(UDPSimplePacketComs* connection, String * name) {
	myName = name;
	myconnection = connection;
	myconnection->connect();
	readController = new AbstractPacketType(1776, 64);
	for (int i = 0; i < name->length(); i++) {
		readController->getDownstreamBytes()[i] = name->charAt(i);
	}
	myconnection->addPollingPacket(readController);
}

UdpNameSearch::~UdpNameSearch() {
	myconnection->disconnect();
}

/**
 * Update the controller
 */
void UdpNameSearch::loop() {
	myconnection->loop(millis(), 1000);

}
void UdpNameSearch::oneShotMode() {
	readController->maxRuns = 1;
	readController->runCount = 0;
}
void UdpNameSearch::continousShotMode() {
	readController->maxRuns = LONG_MAX;
	readController->runCount = 0;
}

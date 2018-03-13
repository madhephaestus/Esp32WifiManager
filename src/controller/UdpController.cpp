/*
 * UdpController.cpp
 *
 *  Created on: Mar 6, 2018
 *      Author: hephaestus
 */

#include "UdpController.h"
#include <client/AbstractPacketType.h>

UdpController::UdpController(UDPSimplePacketComs* connection) {
	myconnection = connection;
	myconnection->connect();
	readController = new AbstractPacketType(1970, 64);
	myconnection->addPollingPacket(readController);
	readController->setResponseListener(this);
}

UdpController::~UdpController() {
	myconnection->disconnect();
}

int UdpController::getId() {
	return readController->getUpstreamBytes()[0];
}

/**
 * Update the controller
 */
void UdpController::loop() {
	myconnection->loop(millis(), 1000);

}
void UdpController::oneShotMode() {
	readController->maxRuns = 1;
	readController->runCount = 0;
}
void UdpController::continousShotMode() {
	readController->maxRuns = LONG_MAX;
	readController->runCount = 0;
}
/**
 * Returns an array of byte data with each byte representing one controller axis value
 */
uint8_t * UdpController::getData() {
	return readController->getUpstreamBytes() + 1;
}
/**
 * Returns an array of byte data with each byte representing one controller axis value
 */
uint8_t * UdpController::getStatus() {
	return readController->getDownstreamBytes();
}
void UdpController::onTimeout(int timeBetweenSendAndRecive) {
	Serial.println(
			"\n\nController Timed OUT! " + String(timeBetweenSendAndRecive));
	isTimedOutValue=true;

}
/**
 * Get the controller timeout state
 */
bool UdpController::isTimedOut() {
	return isTimedOutValue;
}

void UdpController::onResponse(int timeBetweenSendAndRecive) {
	isTimedOutValue=false;

//	Serial.println("\n\nController Responded in "+String(timeBetweenSendAndRecive));
//	for(int i=0;i<20;i++){
//		Serial.println("Val "+String(i)+" = "+String(getData()[i]));
//	}
}

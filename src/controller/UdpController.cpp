/*
 * UdpController.cpp
 *
 *  Created on: Mar 6, 2018
 *      Author: hephaestus
 */

#include "UdpController.h"
#include <client/BytePacketType.h>

UdpController::UdpController(UDPSimplePacketComs* connection) {
	myconnection=connection;
	myconnection->connect();
	readController=new BytePacketType(1970, 64);
	myconnection->addPollingPacket(readController);
	readController->setResponseListener(this);
}

UdpController::~UdpController() {
	myconnection->disconnect();
}

/**
 * Update the controller
 */
void UdpController::loop(){
	myconnection->loop(millis(),1000);

}
/**
 * Returns an array of byte data with each byte representing one controller axis value
 */
uint8_t * UdpController::getData(){
	return readController->getUpstreamBytes();
}

void UdpController::onTimeout( int timeBetweenSendAndRecive){
	Serial.println("\n\nController Timed OUT! "+String(timeBetweenSendAndRecive));

}

void UdpController::onResponse( int timeBetweenSendAndRecive){
	Serial.println("\n\nController Responded in "+String(timeBetweenSendAndRecive));
	for(int i=0;i<20;i++){
		Serial.println("Val "+String(i)+" = "+String(getData()[i]));
	}
}

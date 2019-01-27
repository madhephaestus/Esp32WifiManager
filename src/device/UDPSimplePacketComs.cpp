/*
 * DUPSimplePacketComs.cpp
 *
 *  Created on: Mar 6, 2018
 *      Author: hephaestus
 */

#include "UDPSimplePacketComs.hpp"
static WiFiUDP * udp = NULL;
static boolean connected = false;
std::vector<IPAddress*> availibleIPs;

std::vector<IPAddress*> * getAvailibleIPs() {
	return &availibleIPs;
}

UDPSimplePacketComs::UDPSimplePacketComs(IPAddress* target, boolean useClient) {
	targetDevice = new IPAddress();
	targetDevice->fromString(target->toString());
	if (udp == NULL) {
		udp = new WiFiUDP();
		if (useClient)
			udp->begin(WiFi.localIP(), SIMPLE_PACKET_UDP_PORT);
		else
			udp->begin(WiFi.softAPIP(), SIMPLE_PACKET_UDP_PORT);
		connected = true;
	}
}
/**
 * Abstract methods
 */
/**
 * Set up the device connection
 */
bool UDPSimplePacketComs::connectDeviceImp() {

	return connected;
}
/**
 * Shut down the device connection to be ready to connect again
 */
bool UDPSimplePacketComs::disconnectDeviceImp() {

	return connected;
}
/**
 * A call back to the device to check its read state
 * If the device is not ready no other opperations should take place
 */
bool UDPSimplePacketComs::isReadyDeviceImp() {

	return connected;
}
/**
 * Blocking read with timeout. This should wait for a reasonably long amount of time (200 ms)
 * If time is exceeded return 0
 */
int UDPSimplePacketComs::read(uint8_t* message) {
	if (!connected) {
		return 0;
	}
	return udp->read(message, 64);
}
/**
 * Blocking write with timeout. This should wait for a reasonably long amount of time (200 ms)
 * If time is exceeded return 0
 */
int UDPSimplePacketComs::write(uint8_t * message, int length) {
	if (!connected) {
		return 0;
	}
	if (!udp->beginPacket(*targetDevice, SIMPLE_PACKET_UDP_PORT)) {
		return 0;
	}

	int ret = udp->write(message, length);
	if (udp->endPacket()) {
		//Serial.println("\nSent packet "+ String(ret));
		return ret;
	}
	return 0;
}
/**
 * Non blocking function to check if a packet is availible from the physical layer
 */
bool UDPSimplePacketComs::isResponseReady() {
	if (!connected) {
		return false;
	}
	udp->parsePacket();

	if (udp->available() > 0) {
		//Serial.println("Got packet packet from " );
		//

		boolean found = false;
		IPAddress * tmp; //= new IPAddress();

		for (std::vector<IPAddress*>::iterator it = availibleIPs.begin();
				it != availibleIPs.end(); ++it) {
			tmp = (*it);
			if (tmp[0] == udp->remoteIP())
				found = true;
		}
		if (!found) {
			tmp = new IPAddress();
			tmp->fromString(udp->remoteIP().toString());
			availibleIPs.push_back(tmp);
			Serial.println("\nStoring packet from ");
			Serial.println(udp->remoteIP());
		}
		if (targetDevice[0] == udp->remoteIP()) {
			//Serial.println("Received packet from " );
			return true;
		} else
			Serial.println("Received packet, not for me ");
		if (targetDevice[0][3] == 255) {
			Serial.println("Broadcast Listener dumping ");
			while (udp->read() >= 0)
				;
		}
	}
	return false;
}
//wifi event handler
void UDPSimplePacketComs::WiFiEvent(WiFiEvent_t event) {
	switch (event) {
	case SYSTEM_EVENT_STA_GOT_IP:/**< ESP32 station got IP from connected AP */
		//initializes the UDP state
		//This initializes the transfer buffer
		udp->begin(WiFi.localIP(), SIMPLE_PACKET_UDP_PORT);
		connected = true;
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED: /**< ESP32 station disconnected from AP */
		connected = false;
		break;
	default:
		break;
	}
}

/*
 * DUPSimplePacketComs.cpp
 *
 *  Created on: Mar 6, 2018
 *      Author: hephaestus
 */

#include "UDPSimplePacketComs.hpp"

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
	long start = millis();

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
	if (!udp->beginPacket(targetDevice[0], SIMPLE_PACKET_UDP_PORT)) {
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
		//Serial.println("\nReceived packet");
		return true;
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
	case SYSTEM_EVENT_AP_STACONNECTED: /**< a station connected to ESP32 soft-AP */
		if (!connected)
			udp->begin(WiFi.softAPIP(), SIMPLE_PACKET_UDP_PORT);
		connected = true;
		break;
	default:
		break;
	}
}

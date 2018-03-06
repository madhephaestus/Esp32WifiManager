#include "UDPSimplePacket.h"

UDPSimplePacket::UDPSimplePacket(WiFiUDP * incomingUdp) {
	udp = incomingUdp;
	connected = false;
}
UDPSimplePacket::UDPSimplePacket() {
	udp = new WiFiUDP();
	connected = false;
}

/**
 * Non blocking function to check if a packet is availible from the physical layer
 */
bool UDPSimplePacket::isPacketAvailible() {
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
/**
 * User function that fills the buffer from the data in the physical layer
 * this data should already be framed, checksummed and validated as a valid packet
 */
int32_t UDPSimplePacket::getPacket(uint8_t * buffer, uint32_t numberOfBytes) {
	if (!connected) {
		return 0;
	}
	return udp->read(buffer, numberOfBytes);
}
/**
 * User function that sends the buffer to the physical layer
 * this data should already be framed, checksummed and validated as a valid packet
 */
int32_t UDPSimplePacket::sendPacket(uint8_t * buffer, uint32_t numberOfBytes) {
	if (!connected) {
		return 0;
	}
	if (!udp->beginPacket()) {
		return 0;
	}

	int ret = udp->write(buffer, numberOfBytes);
	if (udp->endPacket()){
		//Serial.println("\nSent packet "+ String(ret));
		return ret;
	}
	return 0;
}

//wifi event handler
void UDPSimplePacket::WiFiEvent(WiFiEvent_t event) {
	switch (event) {
	case SYSTEM_EVENT_STA_GOT_IP:/**< ESP32 station got IP from connected AP */
		//initializes the UDP state
		//This initializes the transfer buffer
		udp->begin(WiFi.localIP(), UDESIMPLEPORT);
		connected = true;
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED: /**< ESP32 station disconnected from AP */
		connected = false;
		break;
	default:
		break;
	}
}


#ifndef UDPPACKET
#define UDPPACKET
#include <SimplePacketComs.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <device/UDPSimplePacketComs.hpp>
class UDPSimplePacket: public SimplePacketComsAbstract {
private:
	WiFiUDP * udp;
public:
	UDPSimplePacket();

	UDPSimplePacket(WiFiUDP * incomingUdp);
	/**
	 * Non blocking function to check if a packet is availible from the physical layer
	 */
	bool isPacketAvailible();
	/**
	 * User function that fills the buffer from the data in the physical layer
	 * this data should already be framed, checksummed and validated as a valid packet
	 */
	int32_t getPacket(uint8_t * buffer, uint32_t numberOfBytes);
	/**
	 * User function that sends the buffer to the physical layer
	 * this data should already be framed, checksummed and validated as a valid packet
	 */
	int32_t sendPacket(uint8_t * buffer, uint32_t numberOfBytes);
	/**
	 * Wifi event function
	 */
	void WiFiEvent(WiFiEvent_t event);

	boolean connected = false;

};

#endif /* end of include guard: UDPPACKET */

/*
 * UDPClient.hpp
 *
 *  Created on: Mar 6, 2018
 *      Author: hephaestus
 */

#ifndef UDPCLIENT
#define UDPCLIENT
#define SIMPLE_PACKET_UDP_PORT 1865

#include <WiFi.h>
#include <WiFiUdp.h>
#include <client/AbstractSimpleComsDevice.h>

class UDPSimplePacketComs: public AbstractSimpleComsDevice {
private:
	WiFiUDP * udp;
	IPAddress* targetDevice;
public:
	UDPSimplePacketComs(IPAddress* target) {
		targetDevice=target;
		udp=new WiFiUDP();
	}

	~UDPSimplePacketComs() {
		disconnect();
	}
	/**
	 * call to shut down the device
	 */
	void disconnect();
	/**
	 * Call to set up the device
	 */
	bool connect();
	/**
	 * Add a packet to the polling queue
	 */
	void addPollingPacket(AbstractPacketType * packet);
	/**
	 * Pass each item in the polling queue through the processor
	 * This is a blocking synchronous function
	 */
	void loop(long millis, long timout);

	/**
	 * Abstract methods
	 */
	/**
	 * Set up the device connection
	 */
	bool connectDeviceImp();
	/**
	 * Shut down the device connection to be ready to connect again
	 */
	bool disconnectDeviceImp();
	/**
	 * A call back to the device to check its read state
	 * If the device is not ready no other opperations should take place
	 */
	bool isReadyDeviceImp();
	/**
	 * Blocking read with timeout. This should wait for a reasonably long amount of time (200 ms)
	 * If time is exceeded return 0
	 */
	int read(uint8_t* message);
	/**
	 * Blocking write with timeout. This should wait for a reasonably long amount of time (200 ms)
	 * If time is exceeded return 0
	 */
	int write(uint8_t * message, int length);
	/**
	 * Wifi event function
	 */
	void WiFiEvent(WiFiEvent_t event);

	/**
	 * Check device to see if a response is ready
	 */
	bool isResponseReady();

	boolean connected = false;

};

#endif /* LIBRARIES_ESPWII_SRC_DEVICE_UDPCLIENT_HPP_ */

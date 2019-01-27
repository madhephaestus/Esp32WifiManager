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
std::vector<IPAddress*> * getAvailibleIPs();

class UDPSimplePacketComs: public AbstractSimpleComsDevice {

public:
	IPAddress* targetDevice;
	UDPSimplePacketComs(IPAddress* target, boolean useClient);

	~UDPSimplePacketComs() {
		disconnect();
	}

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

};

#endif /* LIBRARIES_ESPWII_SRC_DEVICE_UDPCLIENT_HPP_ */

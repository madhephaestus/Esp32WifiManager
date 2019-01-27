/*
 * UdpController.h
 *
 *  Created on: Mar 6, 2018
 *      Author: hephaestus
 */

#ifndef LIBRARIES_UdpNameSearch_H_
#define LIBRARIES_UdpNameSearch_H_
#include <device/UDPSimplePacketComs.hpp>
#include <client/AbstractPacketType.h>
class UdpNameSearch {
	UDPSimplePacketComs* myconnection;
	AbstractPacketType * readController;
	bool isTimedOutValue = false;
	String * myName;
public:
	UdpNameSearch(UDPSimplePacketComs* connection, String * name);
	virtual ~UdpNameSearch();
	/**
	 * Update the controller
	 */
	void loop();

	void oneShotMode();
	void continousShotMode();

};

#endif /* LIBRARIES_ESPWII_SRC_CONTROLLER_UDPCONTROLLER_H_ */

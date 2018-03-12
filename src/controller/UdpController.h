/*
 * UdpController.h
 *
 *  Created on: Mar 6, 2018
 *      Author: hephaestus
 */

#ifndef LIBRARIES_ESPWII_SRC_CONTROLLER_UDPCONTROLLER_H_
#define LIBRARIES_ESPWII_SRC_CONTROLLER_UDPCONTROLLER_H_
#include "AbstractController.h"
#include <device/UDPSimplePacketComs.hpp>
#include <client/IPacketResponseEvent.h>
#include <client/AbstractPacketType.h>
class UdpController:public AbstractController,public IPacketResponseEvent
{
	UDPSimplePacketComs* myconnection;
	AbstractPacketType * readController;
public:
	UdpController(UDPSimplePacketComs* connection);
	virtual ~UdpController();
	/**
	 * Update the controller
	 */
	void loop();
	/**
	 * Returns an array of byte data with each byte representing one controller axis value
	 */
	uint8_t * getData();

	void onResponse( int timeBetweenSendAndRecive);
	void onTimeout( int timeBetweenSendAndRecive);
	void oneShotMode();
	void continousShotMode();
	int getId();

};

#endif /* LIBRARIES_ESPWII_SRC_CONTROLLER_UDPCONTROLLER_H_ */

/*
 * UdpController.h
 *
 *  Created on: Mar 6, 2018
 *      Author: hephaestus
 */

#ifndef LIBRARIES_ESPWII_SRC_CONTROLLER_LocalCONTROLLER_H_
#define LIBRARIES_ESPWII_SRC_CONTROLLER_LocalCONTROLLER_H_
#include "AbstractController.h"
#include <client/IPacketResponseEvent.h>
#include <client/AbstractPacketType.h>
#include <WiiChuck.h>
class LocalController: public AbstractController {
	Accessory * controller;
	bool isTimedOutValue = false;
	uint8_t status[CONTROLLER_BUFFER_SIZE];
	uint8_t data[CONTROLLER_BUFFER_SIZE];

public:
	LocalController(Accessory * toUseController) {
		controller = toUseController;
		controller->begin();
		Serial.println("Creating local Controller");
		for (int i = 0; i < CONTROLLER_BUFFER_SIZE; i++) {
			data[i] = 0;

		}
	}
	/**
	 * Update the controller
	 */
	void loop() {
		controller->readData();
		for (int i = 0; i < WII_VALUES_ARRAY_SIZE; i++) {
			data[i] = controller->getValues()[i];

		}
	}
	/**
	 * Returns an array of byte data with each byte representing one controller axis value
	 */
	uint8_t * getData() {

		return data;
	}
	/**
	 * Returns an array of byte data with each byte representing one  value for upstream display
	 */
	uint8_t * getStatus() {
		return status;
	}
	/**
	 * Get the controller ID
	 */
	int getId() {
		return 0;
	}
	/**
	 * Get the controller timeout state
	 */
	bool isTimedOut() {
		return false;
	}

};

#endif /* LIBRARIES_ESPWII_SRC_CONTROLLER_LocalCONTROLLER_H_ */

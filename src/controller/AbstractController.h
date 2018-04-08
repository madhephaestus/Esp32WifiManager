/*
 * AbstractController.h
 *
 *  Created on: Mar 6, 2018
 *      Author: hephaestus
 */

#ifndef ABSTRACTCONTROLLER
#define ABSTRACTCONTROLLER
#include <stdint.h>
#define CONTROLLER_BUFFER_SIZE 60

class AbstractController {
public:
	/**
	 * Update the controller
	 */
	virtual void loop()=0;
	/**
	 * Returns an array of byte data with each byte representing one controller axis value
	 */
	virtual uint8_t * getData()=0;
	/**
	 * Returns an array of byte data with each byte representing one  value for upstream display
	 */
	virtual uint8_t * getStatus()=0;
	/**
	 * Get the controller ID
	 */
	virtual int getId()=0;
	/**
	 * Get the controller timeout state
	 */
	virtual bool isTimedOut()=0;
};

#endif /* LIBRARIES_ESPWII_SRC_CONTROLLER_ABSTRACTCONTROLLER_H_ */

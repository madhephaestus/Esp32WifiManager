/*
 * AbstractController.h
 *
 *  Created on: Mar 6, 2018
 *      Author: hephaestus
 */

#ifndef ABSTRACTCONTROLLER
#define ABSTRACTCONTROLLER
#include <stdint.h>

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

};

#endif /* LIBRARIES_ESPWII_SRC_CONTROLLER_ABSTRACTCONTROLLER_H_ */

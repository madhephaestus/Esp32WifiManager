/*
 * ServerStatus.h
 *
 *  Created on: Mar 17, 2018
 *      Author: hephaestus
 */

#ifndef LIBRARIES_ESPWII_SRC_SERVER_SERVERSTATUS_H_
#define LIBRARIES_ESPWII_SRC_SERVER_SERVERSTATUS_H_
#define WII_CLASSIC_SERVER 1970

#include <stdint.h>

uint8_t * getControllerAnalogPins();
bool getControllerStatusStartup();
void setControllerStatusStartup(bool stat);
uint8_t * getControllerStatus();
int getNumerOfAnalog();

#endif /* LIBRARIES_ESPWII_SRC_SERVER_SERVERSTATUS_H_ */

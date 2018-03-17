/*
 * ServerStatus.c
 *
 *  Created on: Mar 17, 2018
 *      Author: hephaestus
 */

#include "ServerStatus.h"
#include <stdint.h>

static uint8_t  status[60];
static uint8_t  analogPins[6] = {36,39,34,35,32,33};
static int numAnalog = sizeof(analogPins);
static bool startup = false;

int getNumerOfAnalog(){
	return numAnalog;
}
uint8_t * getControllerStatus(){
	return status;
}
uint8_t * getControllerAnalogPins(){
	return analogPins;
}
bool getControllerStatusStartup(){
	return startup;
}
void setControllerStatusStartup(bool stat){
	startup=stat;
}



/*
 * WiiClassicServer.cpp
 *
 *  Created on: Mar 5, 2018
 *      Author: hephaestus
 */

#include "WiiClassicServer.h"

void WiiClassicServerEvent::event(float * buffer) {
	//Serial.print("\r\nClassic read");
	classic->readData();
	//classic->printInputs();
	if(getControllerStatusStartup()){
		setControllerStatusStartup(true);
		analogReadResolution(8);
		for(int i=0;i<getNumerOfAnalog();i++){
			pinMode(getControllerAnalogPins()[i],INPUT);
		}
	}

	int8_t * charBuff = (int8_t *)buffer;
	for(int i=0;i<60;i++){
		getControllerStatus()[i]=charBuff[i];
	}
	charBuff[0]=controllerIndex;
	uint8_t * values= classic->values;
	for(int i=0;i<WII_VALUES_ARRAY_SIZE;i++){
		charBuff[1+i]=values[i];
	}
	for(int i=0;i<getNumerOfAnalog();i++){
		charBuff[WII_VALUES_ARRAY_SIZE+1+i]=analogRead(getControllerAnalogPins()[i]);
	}
}

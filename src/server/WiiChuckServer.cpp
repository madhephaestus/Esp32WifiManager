/*
 * WiiClassicServer.cpp
 *
 *  Created on: Mar 5, 2018
 *      Author: hephaestus
 */

#include "WiiChuckServer.h"

void WiiChuckServerEvent::event(float * buffer) {
	//Serial.print("\r\nClassic read");
	classic->readData();
	classic->printInputs();
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
	charBuff[1]=classic->getJoyX();
	charBuff[2]=classic->getJoyY();
	charBuff[3]=classic->getRollAngle();
	charBuff[4]=classic->getPitchAngle();
	charBuff[5]=classic->getAccelX();
	charBuff[6]=classic->getAccelY();

	charBuff[7]=classic->getAccelZ();
	charBuff[8]=classic->getButtonC();
	charBuff[9]=classic->getButtonZ();
//	charBuff[10]=classic->getPadLeft();
//	charBuff[11]=classic->getButtonX();
//	charBuff[12]=classic->getButtonY();
//	charBuff[13]=classic->getButtonA();
//	charBuff[14]=classic->getButtonB();
//
//	charBuff[15]=classic->getButtonMinus();
//	charBuff[16]=classic->getButtonHome();
//	charBuff[17]=classic->getButtonPlus();
//
//	charBuff[18]=classic->getButtonZLeft();
//	charBuff[19]=classic->getButtonZRight();
	for(int i=0;i<getNumerOfAnalog();i++){
		charBuff[20+i]=analogRead(getControllerAnalogPins()[i]);
	}
}

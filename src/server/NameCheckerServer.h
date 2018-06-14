/*
 * NameCheckerServer.h
 *
 *  Created on: Jun 13, 2018
 *      Author: hephaestus
 */

#ifndef LIBRARIES_ESP32SIMPLEPACKETCOMS_SRC_SERVER_NAMECHECKERSERVER_H_
#define LIBRARIES_ESP32SIMPLEPACKETCOMS_SRC_SERVER_NAMECHECKERSERVER_H_
#include <SimplePacketComs.h>
#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <Arduino.h>

class NameCheckerServer: public PacketEventAbstract {
private:
	String * namePointer;

public:
	NameCheckerServer(String *robot);
	virtual ~NameCheckerServer();
	void event(float * buffer);
};

#endif /* LIBRARIES_ESP32SIMPLEPACKETCOMS_SRC_SERVER_NAMECHECKERSERVER_H_ */

/*
 * NameCheckerServer.cpp
 *
 *  Created on: Jun 13, 2018
 *      Author: hephaestus
 */

#include "NameCheckerServer.h"

NameCheckerServer::NameCheckerServer(String *robot) :
		PacketEventAbstract(1776) // Address of this event
{
	namePointer = robot;
}
//User function to be called when a packet comes in
// Buffer contains data from the packet coming in at the start of the function
// User data is written into the buffer to send it back
void NameCheckerServer::event(float * buffer) {
	if (namePointer == NULL) {
		return;
	}
	char * bytes = (char *) buffer;

	for (int i = 0; i < namePointer->length(); i++) {
		if (bytes[i] == '*') {
			break; // if we match up to a wildcard, assume match
		}
		if (bytes[i] != namePointer->charAt(i) && bytes[i] != 0xFF) {
			Serial.print("\r\nFailed name check " + namePointer[0] + " got: ");
			for (int j = 0; j < namePointer->length(); j++) {
				Serial.print(bytes[j]);
			}
			noResponse = true; //Name check failed
			return;
		}
	}
	int i;
	for (i = 0; i < namePointer->length() && i < 60; i++) {
		bytes[i] = namePointer->charAt(i);
	}
	for (i = namePointer->length(); i < 60; i++) {
		bytes[i] = 0;
	}
	Serial.print("\r\nSuccess name check " + namePointer[0]);
}
NameCheckerServer::~NameCheckerServer() {
	// TODO Auto-generated destructor stub
}


/*
 * WiiClassicServer.h
 *
 *  Created on: Mar 5, 2018
 *      Author: hephaestus
 */

#ifndef ESWPIISERV
#define ESWPIISERV
#include <PacketEvent.h>
#include <cmath>        // std::abs
#include <WiiChuck.h>

#define WII_CLASSIC_SERVER 1970

class WiiClassicServerEvent: public PacketEventAbstract {
private:
	uint8_t controllerIndex;
	void start(){
		//classic->begin();
	}
public:
	Classic * classic;
	// Packet ID needs to be set
	WiiClassicServerEvent(Classic * incomingClassic,uint8_t mycontrollerIndex) :
			PacketEventAbstract(WII_CLASSIC_SERVER) {
		classic = incomingClassic;
		controllerIndex = mycontrollerIndex;
		start();

	}
	WiiClassicServerEvent(uint8_t mycontrollerIndex) :
			PacketEventAbstract(WII_CLASSIC_SERVER) {
		classic = new Classic();
		controllerIndex = mycontrollerIndex;
		start();

	}

	//User function to be called when a packet comes in
	// Buffer contains data from the packet coming in at the start of the function
	// User data is written into the buffer to send it back
	void event(float * buffer);
};

#endif /* ESWPIISERV */

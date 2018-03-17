#include <WiiChuck.h>
#include <SimplePacketComs.h>
#include <EspWii.h>
#include <WiFi.h>


// WiFi network name and password:
const char *networkName = "RBE_Team_1";
const char *networkPswd = "thisissecret";
#define CONTROLLER_ID 2
//Classic controller;
Nunchuck nunchuck;
void setup() {
	//launchControllerServer(networkName, networkPswd,new WiiClassicServerEvent(&controller,CONTROLLER_ID));
	launchControllerServer(networkName, networkPswd,new WiiChuckServerEvent(&nunchuck,CONTROLLER_ID));

}

void loop() {
	loopServer();
	Serial.println("\n\n");
	for(int i=0;i<60;i++){
		Serial.println("Val "+String(i)+" = "+String( getControllerStatus()[i]));
	}
}

#include <WiiChuck.h>
#include <SimplePacketComs.h>
#include <EspWii.h>
#include <WiFi.h>


// WiFi network name and password:
const char *networkName = "RBE_Team_1";
const char *networkPswd = "thisissecret";
void setup() {
	launchControllerServer(networkName, networkPswd);
}

void loop() {
	loopServer();
//	for(int i=0;i<60;i++){
//		Serial.println("Val "+String(i)+" = "+String( getControllerStatus()[i]));
//	}
}

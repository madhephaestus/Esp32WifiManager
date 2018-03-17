#include <WiiChuck.h>
#include <SimplePacketComs.h>
#include <EspWii.h>
#include <WiFi.h>


// WiFi network name and password:
const char *networkName = "RBE_Team_1";
const char *networkPswd = "thisissecret";
#define CONTROLLER_ID 2
void setup() {
	//launchControllerServer(networkName, networkPswd,new WiiClassicServerEvent(CONTROLLER_ID));
	launchControllerServer(networkName, networkPswd,new WiiChuckServerEvent(CONTROLLER_ID));

}

void loop() {
	loopServer();
	//Serial.println("\n\n");
	//for(int i=0;i<60;i++){
	//	Serial.println("Val "+String(i)+" = "+String( getControllerStatus()[i]));
	//}
}

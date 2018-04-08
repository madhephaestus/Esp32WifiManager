#include <SimplePacketComs.h>
#include <WiiChuck.h>
#include <WiFi.h>
#include <EspWii.h>
#include <ESP32Servo.h>

// WiFi network name and password:
const char *networkName = "RBE_Team_1";
const char *networkPswd = "thisissecret";
#define CONTROLLER_ID 2
//Nunchuck * nun= new Accessory();
Accessory  classic;
long timeSincePrint=0;
void setup() {
	launchControllerServer(networkName, networkPswd,new WiiClassicServerEvent(&classic,CONTROLLER_ID));
	//launchControllerServer(networkName, networkPswd,new WiiChuckServerEvent(nun,CONTROLLER_ID));
	//classic.type=WIICLASSIC;

}

void loop() {
	loopServer();
	classic.readData();
	if(millis()-timeSincePrint>100){
		timeSincePrint=millis();
		classic.printInputs();
		Serial.println("\n\n");
		for(int i=0;i<60;i++){
			//Serial.println("Val "+String(i)+" = "+String( getControllerStatus()[i]));
		}
		for(int i=0;i<WII_VALUES_ARRAY_SIZE;i++){
			Serial.println("Controller Val "+String(i)+" = "+String( (uint8_t)classic.values[i]));
		}
	}
}

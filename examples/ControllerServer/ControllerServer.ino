#include <SimplePacketComs.h>
#include <WiiChuck.h>
#include <WiFi.h>
#include <EspWii.h>
#include <ESP32Servo.h>

// WiFi network name and password:
#define AP_SSID  "RBE_Team_1"       //can set ap hostname here
#define AP_PW	 "thisissecret"       //can set ap hostname here
#define CONTROLLER_ID 2
#define TEAMNUMBER    21
String * controllerName = new String("GameController_"+String(TEAMNUMBER));

Accessory  classic;
long timeSincePrint=0;
void setup() {
	launchControllerServer(AP_SSID, AP_PW,new WiiClassicServerEvent(&classic,CONTROLLER_ID));
	setNameUdpDevice(controllerName);

}

void loop() {
	loopServer();
	classic.readData();
	/*
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
	*/
}

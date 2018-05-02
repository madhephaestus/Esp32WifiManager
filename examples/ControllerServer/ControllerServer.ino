#include <SimplePacketComs.h>
#include <WiiChuck.h>
#include <WiFi.h>
#include <EspWii.h>
#include <ESP32Servo.h>

#define CONTROLLER_ID 2
#define TEAMNUMBER    22
String * controllerName = new String("GameController_"+String(TEAMNUMBER));

Accessory  classic;
long timeSincePrint=0;
void setup() {
	launchControllerServer();
	addServer(new WiiClassicServerEvent(&classic,CONTROLLER_ID));
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

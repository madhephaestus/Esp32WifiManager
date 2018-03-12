#include <EspWii.h>
#define AP_SSID  "RBE_Team_1"       //can set ap hostname here
#define AP_PW	 "thisissecret"       //can set ap hostname here
AbstractController * gameController=NULL;
Servo m1;
Servo m2;
//The setup function is called once at startup of the sketch
void setup()
{
	launchControllerReciver(AP_SSID,AP_PW);
	Serial.println("Attaching servos...");
	m1.attach(2);
	m2.attach(15);
}

// The loop function is called in an endless loop
void loop()
{
	loopReciver();

	if(gameController!=NULL){
		//user code
		Serial.println("Code running..");
		m1.write((int)(gameController->getData()[1]<<2));
		m2.write((int)(gameController->getData()[3]<<3));

	}else
		gameController=getUdpController(2);
}

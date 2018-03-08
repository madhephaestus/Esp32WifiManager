#include <EspWii.h>
#define AP_SSID  "RBE_Team_1"       //can set ap hostname here
#define AP_PW	 "thisissecret"       //can set ap hostname here
AbstractController * gameController=NULL;
//The setup function is called once at startup of the sketch
void setup()
{
	launchControllerReciver(AP_SSID,AP_PW);
}

// The loop function is called in an endless loop
void loop()
{
	loopReciver();

	if(gameController!=NULL){
		//user code
		Serial.println("Code running..");

	}else
		gameController=getUdpController(2);
}

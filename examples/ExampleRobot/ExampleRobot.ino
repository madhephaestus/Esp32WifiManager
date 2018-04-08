#include <SimplePacketComs.h>
#include <WiiChuck.h>
#include <WiFi.h>
#include <EspWii.h>
#include <ESP32Servo.h>
#include <gamelogic/GameLogic.h>
#define AP_SSID  "RBE_Team_1"       //can set ap hostname here
#define AP_PW	 "thisissecret"       //can set ap hostname here
//#define CONTROLLER_ID 2 // use remote controller from WiFi
#define CONTROLLER_ID 0 // use local Wii controller
class MyRobot:public AbstractRobot{
private:
	Servo m1;
	Servo m2;
public:
	/**
	 * Called when the start button is pressed and the robot control begins
	 */
	void robotStartup(){
		Serial.println("Attaching servos...");
		m1.attach(2,1000,2000);
		m2.attach(16,1000,2000);
	}
	/**
	 * Called by the controller between communication with the wireless controller
	 * during autonomous mode
	 * @param time the amount of time remaining
	 * @param dfw instance of the DFW controller
	 */
	void autonomous( long time,uint8_t * status){
		Serial.println("Auto: time remaining: "+String(time));

	}
	/**
	 * Called by the controller between communication with the wireless controller
	 * during teleop mode
	 * @param time the amount of time remaining
	 * @param dfw instance of the DFW controller
	 */
	void teleop( long time,uint8_t * status, uint8_t * data){
		int servoValue1 = map(data[1],0,255,0,180);
		int servoValue2 = map(data[0],0,255,0,180);
		m1.write(servoValue1);
		m2.write(servoValue2);
		//Serial.println("Servos updated with: "+String(servoValue1)+" "+String(servoValue2)+" time remaining: "+String(time));

	}
	/**
	 * Called at the end of control to reset the objects for the next start
	 */
	void robotShutdown(void){
		Serial.println("Robot to safe state");
		m1.write(90);
		m2.write(90);

		m1.detach();
		m2.detach();
	}
	/**
	 * Return the number of the LED used for controller signaling
	 */
	int getDebugLEDPin(void){
		return 13;
	}
};

GameLogic logic(new MyRobot(),CONTROLLER_ID);

//The setup function is called once at startup of the sketch
void setup()
{
	launchControllerReciver(AP_SSID,AP_PW);

}

// The loop function is called in an endless loop
void loop()
{
	logic.run();
	//logic.printState();

	// to force teleop
	switch (logic.state){
	case Teleop:
	case powerup :
		return;// if starting up or teleop is running, do nothing
	default:
		logic.printState();
		logic.state =startTeleop;// force a start of teleop over and over
	}

	/*
	// to force Autonomous
	switch (logic.state){
	case Autonomous:
	case powerup :
		return;// if starting up or Autonomous is running, do nothing
	default:
		logic.state =startAuto;// force a start of Autonomous over and over
	}
	*/

}

#include <SimplePacketComs.h>
#include <WiiChuck.h>
#include <WiFi.h>
#include <EspWii.h>
#include <ESP32Servo.h>
#include <gamelogic/GameLogic.h>
#include <math.h>
// Team specific data
#define AP_SSID  "RBE_Team_1"       //can set ap hostname here
#define AP_PW	 "thisissecret"       //can set ap hostname here
#define CONTROLLER_ID 2 // use remote controller from WiFi
//#define CONTROLLER_ID 0 // use local Wii controller
#define TEAMNUMBER    22
String * controllerName = new String("GameController_"+String(TEAMNUMBER));
// Robot definition
class MyRobot: public AbstractRobot {
private:
	Servo m1;
	Servo m2;
	bool attached = false;
	long timeSinceUpdate =0;
	float sinwaveIndex =0;
	float sinwaveMax = 30;
public:
	/**
	 * Called when the start button is pressed and the robot control begins
	 */
	void robotStartup() {
		if (!attached) {
			Serial.println("Attaching servos...");
			m1.attach(2, 1000, 2000);
			m2.attach(16, 1000, 2000);
			attached = true; // ensure this happens once and only once
			/**
			 * Put setup code here
			 */
		}
	}
	/**
	 * Called by the controller between communication with the wireless controller
	 * during autonomous mode
	 * @param status an array of bytes that the user can write to to be displayed on the base station
	 */
	void autonomous(long time, uint8_t * status) {
		//
		for (int i = 0; i < CONTROLLER_BUFFER_SIZE; i++) {
			status[i] = 42;// random data placed in buffer to be printed by base station

		}
		if(millis()-timeSinceUpdate>10){
			timeSinceUpdate=millis();
			int servoValue1 = map((long)(sin(sinwaveIndex/sinwaveMax*2*PI)*127), -127, 127, 20, 160);
			int servoValue2 = map((long)(cos(sinwaveIndex/sinwaveMax*2*PI)*127), -127, 127, 160, 20);
			sinwaveIndex=sinwaveIndex+1;
			if(sinwaveIndex>=sinwaveMax)
				sinwaveIndex=0;
			//Write to the servo
			m1.write(servoValue1);
			m2.write(servoValue2);
		}else{
			//Serial.println("Auto: time remaining: " + String(time));
		}
		/*
		 * put autonomous code here
		 */
	}
	/**
	 * Called by the controller between communication with the wireless controller
	 * during teleop mode
	 * @param time the amount of time remaining
	 * @param status an array of bytes that the user can write to to be displayed on the base station
	 * @param data the array of bytes representing the data from the game controller
	 *
	 */
	void teleop(long time, uint8_t * status, uint8_t * data) {
		// map from controller values to servo values
		int servoValue1 = map(data[1], 0, 255, 0, 180);
		int servoValue2 = map(data[0], 0, 255, 180, 0);
		//Write to the servo
		m1.write(servoValue1);
		m2.write(servoValue2);
//		Serial.println(
//				"Servos updated with: " + String(servoValue1) + " "
//						+ String(servoValue2) + " time remaining: "
//						+ String(time));
		for (int i = 0; i < CONTROLLER_BUFFER_SIZE; i++) {
			status[i] = 37;// random data placed in buffer to be printed by base station

		}
//		for (int i = 0; i < CONTROLLER_BUFFER_SIZE; i++) {
//			Serial.println(String(i) + " = " + String(data[i]));
//
//		}
		/**
		 * put teleop code here
		 */

	}
	/**
	 * Called at the end of control to reset the objects for the next start
	 */
	void robotShutdown(void) {
		Serial.println("Robot to safe state");
		m1.write(90);
		m2.write(90);
		/**
		 * Put shutdown code here
		 */

	}
	/**
	 * Return the number of the LED used for controller signaling
	 */
	int getDebugLEDPin(void) {
		return 13;
	}
};

GameLogic logic(new MyRobot(), CONTROLLER_ID);

//The setup function is called once at startup of the sketch
void setup() {
	launchControllerReciver(AP_SSID, AP_PW, controllerName);
}

// The loop function is called in an endless loop
void loop() {
	logic.run();
//	logic.printState();// display the current state of the game state machine

}

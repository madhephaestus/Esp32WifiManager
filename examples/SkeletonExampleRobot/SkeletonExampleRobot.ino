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

// Robot definition
class MyRobot: public AbstractRobot {
private:
	bool attached = false;
public:
	/**
	 * Called when the start button is pressed and the robot control begins
	 */
	void robotStartup() {
		if (!attached) {
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
		/**
		 * put teleop code here
		 */

	}
	/**
	 * Called at the end of control to reset the objects for the next start
	 */
	void robotShutdown(void) {
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
	launchControllerReciver(AP_SSID, AP_PW);
}

// The loop function is called in an endless loop
void loop() {
	logic.run();
//	logic.printState();// display the current state of the game state machine

}

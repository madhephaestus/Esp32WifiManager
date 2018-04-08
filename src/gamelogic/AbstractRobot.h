/*
 * AbstractDFWRobot.h
 *
 *  Created on: Aug 23, 2017
 *      Author: hephaestus
 */

#ifndef LIBRARIES_DFW_ABSTRACTDFWROBOT_H_
#define LIBRARIES_DFW_ABSTRACTDFWROBOT_H_
#include <gamelogic/GameLogic.h>

class AbstractRobot{
public:
	/**
	 * Called when the start button is pressed and the robot control begins
	 */
	virtual void robotStartup()=0;
	/**
	 * Called by the controller between communication with the wireless controller
	 * during autonomous mode
	 * @param time the amount of time remaining
	 * @param status an array of bytes that the user can write to to be displayed on the base station
	 */
	virtual void autonomous( long time,uint8_t * status)=0;
	/**
	 * Called by the controller between communication with the wireless controller
	 * during teleop mode
	 * @param time the amount of time remaining
	 * @param status an array of bytes that the user can write to to be displayed on the base station
	 * @param data the array of bytes representing the data from the game controller
	 */
	virtual void teleop( long time,uint8_t * status, uint8_t * data)=0;
	/**
	 * Called at the end of control to reset the objects for the next start
	 */
	virtual void robotShutdown(void)=0;
	/**
	 * Return the number of the LED used for controller signaling
	 */
	virtual int getDebugLEDPin(void)=0;
};




#endif /* LIBRARIES_DFW_ABSTRACTDFWROBOT_H_ */

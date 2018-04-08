/**
 * Serial controller library for DFWireless controllers
 * For RBE Class use
 * @author Joe St. Germain joest@wpi.edu
 * @author Alex Taglieri (atags22)
 * @author Kevin Harrington (madhephaestus)
 */
//written by Joe St. Germain
//
//
//
#ifndef DFW_h
#define DFW_h

#include "AbstractRobot.h"
#include "Arduino.h"
#include "controller/AbstractController.h"
#define autoTime 20000
#define teleopTime  120000
#define functionReturnTime  100
enum CompetitionState {
	powerup, waitForAuto,startAuto, Autonomous,waitForTeleop,startTeleop, Teleop
};
#define Null 0
class GameLogic {
public:
	GameLogic(AbstractRobot * myrobot, int controllerIndex);
	void run();
	CompetitionState state;
	void printState();
	AbstractController * controller;

private:
	bool start();
	void startup();
	int controllerIndex=0;
	AbstractRobot * robot;
	void update(void);
	long autoStartTime;
	long teleopStartTime;
	long lastHeartBeatTime;
	long hbTime;
	long flashTime;
};

#endif

#include <SimplePacketComs.h>
#include <WiFi.h>
#include <PacketEvent.h>
#include <Esp32SimplePacketComs.h>
#include <Preferences.h>

enum RobotStatus {
	Ready_for_new_task = 0,
	Heading_to_pickup = 1,
	Picking_up = 2,
	Heading_to_Dropoff = 3,
	Dropping_off = 4,
	Heading_to_safe_zone = 5,
	Fault_failed_pickup = 6,
	Fault_failed_dropoff = 7,
	Fault_excessive_load = 8,
	Fault_obstructed_path = 9,
	Fault_E_Stop_pressed = 10
};

//A robot class to share data between listeners
class MyRobot {

public:
	RobotStatus status;
	MyRobot() {

	}
};

class ClearFaults: public PacketEventAbstract {
	MyRobot* robotPointer;
public:
	// Packet ID needs to be set
	ClearFaults(MyRobot* robot) :
			PacketEventAbstract(1871) // Address of this event
	{
		robotPointer = robot;
	}
	//User function to be called when a packet comes in
	// Buffer contains data from the packet coming in at the start of the function
	// User data is written into the buffer to send it back
	void event(float * buffer) {
		// no data
	}
};
class PickOrder: public PacketEventAbstract {
	MyRobot* robotPointer;

public:
	// Packet ID needs to be set
	PickOrder(MyRobot* robot) :
			PacketEventAbstract(1936)	// Address of this event
	{
		robotPointer = robot;
	}
	//User function to be called when a packet comes in
	// Buffer contains data from the packet coming in at the start of the function
	// User data is written into the buffer to send it back
	void event(float * buffer) {
		int numberOfFloats = 15;
		float pickupArea = buffer[0];
		float pickupX = buffer[1];
		float pickupZ = buffer[2];
		float dropOffArea = buffer[3];
		float dropOffX = buffer[4];
		float dropOffZ = buffer[5];
		Serial.println(
				"Pick Order Recived from : " + String(pickupArea) + " "
						+ String(pickupX) + " " + String(pickupZ) + " to : "
						+ String(dropOffArea) + " " + String(dropOffX) + " "
						+ String(dropOffZ));
	}
};
class DirectDrive: public PacketEventAbstract {
	MyRobot* robotPointer;
	float sessionIDPRevious=0;
public:
	// Packet ID needs to be set
	DirectDrive(MyRobot* robot) :
			PacketEventAbstract(1786)	// Address of this event
	{
		robotPointer = robot;
	}
	//User function to be called when a packet comes in
	// Buffer contains data from the packet coming in at the start of the function
	// User data is written into the buffer to send it back
	void event(float * buffer) {
		int numberOfFloats = 15;
		float deltax = buffer[0];
		float deltaY = buffer[1];
		float deltaZ = buffer[2];
		float deltaAzimuth = buffer[3];
		float deltaElevation = buffer[4];
		float deltaTilt = buffer[5];
		float milisecondsFOrTrasnition = buffer[6];
		float sessionID = buffer[7];
		float status = 0;
		if(sessionIDPRevious!=sessionID){
			sessionIDPRevious=sessionID;
			Serial.println(
					"Direct Drive Recived : " + String(deltax) + " "
							+ String(deltaY) + " " + String(deltaZ) + "  "
							+ String(deltaAzimuth) + " " + String(deltaElevation)
							+ " " + String(deltaTilt)) + " "
					+ String(milisecondsFOrTrasnition) + " " + String(sessionID)
					+ " " + String(status);
		}
		buffer[0] = status;
	}
};
class GetLocation: public PacketEventAbstract {
	MyRobot* robotPointer;

public:
	// Packet ID needs to be set
	GetLocation(MyRobot* robot) :
			PacketEventAbstract(1994)	// Address of this event
	{
		robotPointer = robot;
	}
	//User function to be called when a packet comes in
	// Buffer contains data from the packet coming in at the start of the function
	// User data is written into the buffer to send it back
	void event(float * buffer) {
		int numberOfFloats = 15;
		buffer[0] = 10;	// X Location
		buffer[1] = 0;	// Y Location
		buffer[2] = 0;	// Z Location
		buffer[3] = 45;	// azimuth
		buffer[4] = 0;	// elevation
		buffer[5] = 0;	// tilt
		buffer[6] = 100;	// X size
		buffer[7] = 100;	// Y size
		buffer[8] = 100;	// Z size
//		Serial.println(
//				"Location request " + String(buffer[0]) + " "
//						+ String(buffer[1]) + " " + String(buffer[2]) + " "
//						+ String(buffer[3]) + " " + String(buffer[4]) + " "
//						+ String(buffer[5]) + " " + String(buffer[6]) + " "
//						+ String(buffer[7]) + " " + String(buffer[8]));
	}
};

class GetStatus: public PacketEventAbstract {
	MyRobot* robotPointer;

public:
	// Packet ID needs to be set
	GetStatus(MyRobot* robot) :
			PacketEventAbstract(2012)	// Address of this event
	{
		robotPointer = robot;
	}
	//User function to be called when a packet comes in
	// Buffer contains data from the packet coming in at the start of the function
	// User data is written into the buffer to send it back
	void event(float * buffer) {
		uint8_t * bytes = (uint8_t *) buffer;
		int numberOfBytes = 60;
		bytes[0] = robotPointer->status;
		for (int i = 1; i < numberOfBytes; i++) {
			bytes[i] = 0;	// write a value to the controller
		}
	}
};
MyRobot robot;
static Preferences preferences;

void setup() {
	preferences.begin("wifi", false); // Note: Namespace name is limited to 15 chars
	//preferences.clear();// erase all stored passwords
	//preferences.putString("ssid", "TestNetAP");// set a specific default network to connect to if many possible are present
	// Use serial terminal to set passwords
	// open serial moniter and type the SSID and hit enter
	// when the device prints a prompt for a new password, type the password and hit enter
	preferences.end();
	// Initialize the server mechanism with at least one listener
	launchControllerServer();
	addServer(new ClearFaults(&robot));
	// Add additional servers
	addServer(new PickOrder(&robot));
	// Send and recive bytes instead of floats
	addServer(new GetStatus(&robot));
	// Get the location of the robot
	addServer(new GetLocation(&robot));
	// Direct drive robot command
	addServer(new DirectDrive(&robot));
	setNameUdpDevice(new String("Warehouse-21"));
}

void loop() {
	loopServer();
}

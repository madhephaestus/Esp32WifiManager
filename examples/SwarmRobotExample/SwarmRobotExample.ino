#include <SimplePacketComs.h>
#include <WiiChuck.h>
#include <WiFi.h>
#include <EspWii.h>
#include <ESP32Servo.h>
#include <PacketEvent.h>

// WiFi network name and password:
//#include <auth.h> //put your credential in a file called this and do not commit it to verison control!

const char *networkName = "falcon";
const char *networkPswd = "turtlebot";
//A robot class to share data between listeners
class MyRobot{
public :
	MyRobot(){

	}
};

class JustReadServerEvent: public PacketEventAbstract {
	MyRobot* robotPointer;
public:
	// Packet ID needs to be set
	JustReadServerEvent(MyRobot* robot):
		PacketEventAbstract(1871)// Address of this event
	{
		robotPointer=robot;
	}
	//User function to be called when a packet comes in
	// Buffer contains data from the packet coming in at the start of the function
	// User data is written into the buffer to send it back
	void event(float * buffer){
		int numberOfFloats = 15;
		for(int i=0;i<numberOfFloats;i++){
			buffer[i]=27;// write a value for the controller to read
		}
	}
};
class ReadAndWriteServerEvent: public PacketEventAbstract {
	MyRobot* robotPointer;

public:
	// Packet ID needs to be set
	ReadAndWriteServerEvent(MyRobot* robot):
		PacketEventAbstract(1936)// Address of this event
	{
		robotPointer=robot;
	}
	//User function to be called when a packet comes in
	// Buffer contains data from the packet coming in at the start of the function
	// User data is written into the buffer to send it back
	void event(float * buffer){
		int numberOfFloats = 15;
		for(int i=0;i<numberOfFloats;i++){
			float val = buffer[i];// read a value from the controller
			buffer[i]=17;// write a value for the controller to read
		}
	}
};
class BytePacketServerEvent: public PacketEventAbstract {
	MyRobot* robotPointer;

public:
	// Packet ID needs to be set
	BytePacketServerEvent(MyRobot* robot):
		PacketEventAbstract(2012)// Address of this event
	{
		robotPointer=robot;
	}
	//User function to be called when a packet comes in
	// Buffer contains data from the packet coming in at the start of the function
	// User data is written into the buffer to send it back
	void event(float * buffer){
		uint8_t * bytes = (uint8_t *)buffer;
		int numberOfBytes = 60;
		for(int i=0;i<numberOfBytes;i++){
			uint8_t val = bytes[i];// read a byte from the controller
			bytes[i]=37;// write a value to the controller
		}
	}
};
MyRobot robot;
void setup() {
	//Initialize the server mechanism with at least one listener
	launchControllerServer(networkName, networkPswd,new JustReadServerEvent(&robot));
	//Add additional servers
	addServer(new ReadAndWriteServerEvent(&robot));
	// Send and recive bytes instead of floats
	addServer(new BytePacketServerEvent(&robot));
}

void loop() {
	loopServer();
}

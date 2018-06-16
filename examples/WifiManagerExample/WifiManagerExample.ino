#include <Esp32SimplePacketComs.h>
#include <wifi/WifiManager.h>
#include <SimplePacketComs.h>
#include <Preferences.h>

//Create a wifi manager
WifiManager manager;
// modify the preferences
static Preferences preferences;

void setup() {
	preferences.begin("wifi", false); // Note: Namespace name is limited to 15 chars
	//preferences.clear();// erase all stored passwords
	preferences.end();
	// use the preferences to start up the wifi
	manager.setup();
}

void loop() {
	// read the serial port for new passwords and maintain connections
	manager.loop();
	if (manager.getState() == Connected) {
		// use the Wifi Stack now connected
	}
}

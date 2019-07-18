#include <Preferences.h>
#include <WiFi.h>
#include <Esp32WifiManager.h>

//Create a wifi manager
WifiManager manager;
// modify the preferences
void setup() {
	//manager.erase();// This will erase the stored passwords
	manager.setupScan();
}

void loop() {
	// read the serial port for new passwords and maintain connections
	manager.loop();
	if (manager.getState() == Connected) {
		// use the Wifi Stack now connected
	}
}

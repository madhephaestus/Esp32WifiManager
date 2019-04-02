#include <Preferences.h>
#include <WiFi.h>
#include <Esp32WifiManager.h>

//Create a wifi manager
WifiManager manager;
// modify the preferences
void setup() {
	Preferences preferences;
	preferences.begin("wifi", false); // Note: Namespace name is limited to 15 chars
	// preferences.clear();// erase all stored passwords
	// preferences.putString("hephaestus", "none");// erase a specific default network password
	// Use serial terminal to set passwords
	// open serial moniter and type the SSID and hit enter
	// when the device prints a prompt for a new password, type the password and hit enter
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

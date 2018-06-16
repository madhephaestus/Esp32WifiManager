#include <Esp32SimplePacketComs.h>
#include <wifi/WifiManager.h>
#include <SimplePacketComs.h>
#include <Preferences.h>

//Create a wifi manager
WifiManager manager;
void setup() {
  // use the preferences to start up the wifi
 manager.setup();
}

void loop() {
  // read the serial port for new passwords and maintain connections
  manager.loop();
  if(manager.getState()==Connected){
	  // use the Wifi Stack now connected
  }
}

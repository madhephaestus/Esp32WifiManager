/*
 * WifiManager.h
 *
 *  Created on: Jun 13, 2018
 *      Author: hephaestus
 */

#ifndef LIBRARIES_ESP32SIMPLEPACKETCOMS_SRC_WIFI_WIFIMANAGER_H_
#define LIBRARIES_ESP32SIMPLEPACKETCOMS_SRC_WIFI_WIFIMANAGER_H_
#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>

enum connectionState {
	firstStart,
	Disconnected,
	InitialConnect,
	Connected,
	HaveSSIDSerial,
	reconnect
};

class WifiManager {
private:

	void connectToWiFi(const char * ssid, const char * pwd);
	void startAP() ;
public:
	bool APMode=false;
	//boolean connected = false;
	long timeOfLastDisconnect = 0;
	long timeOfLastConnect = 0;
	String networkNameServer;
	String networkPswdServer;
	String apNameServer;
	String apPswdServer;
	int connectionAttempts;
	WifiManager();
	virtual ~WifiManager();
	void loop();
	void setup();
	void setupAP();
	void rescan();
	void printState();
	enum connectionState getState();
};

#endif /* LIBRARIES_ESP32SIMPLEPACKETCOMS_SRC_WIFI_WIFIMANAGER_H_ */

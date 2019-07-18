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
#include <esp_wifi.h>
#define rescanIncrement 2
#define timeoutTime 2000
enum connectionState {
	firstStart,
	Disconnected,
	InitialConnect,
	apconnected,
	Connected,
	HaveSSIDSerial,
	reconnect,
	APWaitingForSTA,
	scanRunning,
	scanDone
};

class WifiManager {
private:
	void connectToWiFi(const char * ssid, const char * pwd);
	void startAP() ;
	bool APMode=false;
	//boolean connected = false;
	long timeOfLastDisconnect = 0;
	long timeOfLastConnect = 0;
	String networkNameServer;
	String networkPswdServer;
	String apNameServer;
	String apPswdServer;
	int connectionAttempts=0;
	Preferences preferences;
	void rescan();
	long timeSinceAPPrint =0;
	long timeSinceAPStart =0;
	enum connectionState state=firstStart;
	enum connectionState whatToDoAfterScanning=reconnect;
	bool setupDone = false;
	void runSerialLoop();
public:
	/**
	 * Static reference used by teh wifi event to pass teh event from the static context to the object context.
	 */
	static WifiManager * staticRef;
	/**
	 * WifiManager is a class to manage the WIFI connections and reconnections
	 * THis manager will also take over the Serial object and use it to set connection
	 * To connect to an AP, type the AP into the arduino serial moniter and hit enter
	 * When the password prompt comes up, type the password and and hit enter.
	 * if the password is correct, the AP will be stored for a fast connect next time.
	 *
	 * To change the AP name, use ap:MyNewAPName instead of the ap to which you want to connect
	 * the password will be stored as the AP name
	 */
	WifiManager();
	virtual ~WifiManager();
	/**
	 * call the loop function to update the state
	 */
	void loop();
	/**
	 * Perfoem a normal setup
	 * This will attempt to connect to a stored network.
	 * If the stored network is not found, it will re-scan and connect to a remembered network,
	 * if no remembered network is found, it will start AP mode
	 */
	void setup();
	/**
	 * Start the manager with AP mode started by default
	 */
	void setupAP();
	/**
	 * Start the manager but re-scan the environment first
	 */
	void setupScan();
	/**
	 * A formated print statement of the current state
	 */
	void printState();
	/**
	 * Get the state of the wifi manager
	 */
	enum connectionState getState();
	/**
	 * Internal function to process wifi events
	 * This is only public to allow the static wifi event to call it
	 */
	void WiFiEvent(WiFiEvent_t event);
	/**
	 * Check to see if the device is already in AP mode.
	 */
	bool isApMode();

	/**
	 * erase()
	 * Erase all stored passwords and AP mode settings
	 */
	void erase();
	/**
	 * disconnect
	 * Force a disconnection and rescan of the system
	 */
	void disconnect();
	/**
	 * Update AP list
	 *
	 * This function will update the AP list, then reconnect
	 * @return the current number of availible AP's
	 * @Note this will take a few seconds and is BLOCKING during that time
	 */
	int updateApList();
};

#endif /* LIBRARIES_ESP32SIMPLEPACKETCOMS_SRC_WIFI_WIFIMANAGER_H_ */

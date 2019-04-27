/*
 * WifiManager.cpp
 *
 *  Created on: Jun 13, 2018
 *      Author: hephaestus
 */

#include "WifiManager.h"
#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <esp_wifi.h>
static Preferences preferences;
void WiFiEventWifiManager(WiFiEvent_t event);
static WifiManager * staticRef = NULL;
static enum connectionState state;
static bool setupDone = false;
#define rescanIncrement 2
WifiManager::WifiManager() {
	state = firstStart;
	connectionAttempts = 0;
}

WifiManager::~WifiManager() {
	// TODO Auto-generated destructor stub
}
enum connectionState WifiManager::getState() {
	return state;
}
void WifiManager::printState() {
	switch (state) {
	case firstStart:
		Serial.println("WifiManager.getState()=firstStart");
		break;
	case Disconnected:
		Serial.println("WifiManager.getState()=Disconnected");
		break;
	case InitialConnect:
		Serial.println("WifiManager.getState()=InitialConnect");
		break;
	case Connected:
		Serial.println("WifiManager.getState()=Connected");
		break;
	case HaveSSIDSerial:
		Serial.println("WifiManager.getState()=HaveSSIDSerial");
		break;
	case reconnect:
		Serial.println("WifiManager.getState()=reconnect");
		break;
	}
}
void WifiManager::setupAP() {
	APMode = true;
	setup();
}
void WifiManager::setup() {
	if (setupDone)
		return;
	Serial.begin(115200);

	preferences.begin("wifi", true);
	networkNameServer = preferences.getString("ssid", "none");    //NVS key ssid
	networkPswdServer = preferences.getString(networkNameServer.c_str(),
			"none"); //NVS key password
	apNameServer = preferences.getString("apssid", "none"); //NVS key ssid
	if (apNameServer.compareTo("none") == 0) {
		WiFi.mode(WIFI_MODE_AP);
		uint8_t mac[6];
		char macStr[18] = { 0 };
		esp_wifi_get_mac(WIFI_IF_STA, mac);
		sprintf(macStr, "%02X-%02X", mac[4], mac[5]);
		apNameServer = "esp32-" + String(macStr);
	}
	apPswdServer = preferences.getString(apNameServer.c_str(), "Wumpus3742"); //NVS key password
	Serial.println(" AP Mode SSID=" + apNameServer + ":" + apPswdServer);
	preferences.end();
	state = reconnect;
	printState();
	staticRef = this;
	connectionAttempts = 0;
	WiFi.onEvent(WiFiEventWifiManager);
	connectionAttempts = 1;
	setupDone = true;
}
void WifiManager::startAP() {
	WiFi.disconnect(true);
	Serial.println("\n\n\nAP starting " + apNameServer);
//Initiate connection
	WiFi.mode(WIFI_MODE_AP);
	String mac = WiFi.macAddress();
	WiFi.softAP(apNameServer.c_str(), apPswdServer.c_str());

	timeOfLastConnect = 0;
	timeOfLastDisconnect = millis() - rescanIncrement;

	Serial.println("Mac Address: " + mac);

	preferences.begin("wifi", false); // Note: Namespace name is limited to 15 chars
	if (preferences.getString("apssid", "none").compareTo(apNameServer) != 0) {
		preferences.putString("apssid", apNameServer);

		delay(300);
	}
	if (preferences.getString(apNameServer.c_str(), "none").compareTo(
			apPswdServer) != 0) {
		preferences.putString(apNameServer.c_str(), apPswdServer);
		delay(300);
	}
	preferences.end();
	Serial.println(
			"Starting AP '" + apNameServer + "' : '" + apPswdServer + "'");
}
void WifiManager::connectToWiFi(const char * ssid, const char * pwd) {

	Serial.println("Attempting to connect to WiFi network: " + String(ssid));
//Initiate connection
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, pwd);

	Serial.println("Waiting for WIFI connection... ");
	timeOfLastConnect = 0;
	timeOfLastDisconnect = millis() - rescanIncrement;
	String mac = WiFi.macAddress();
	Serial.println("Mac Address: " + mac);
}

void WifiManager::rescan() {
	bool myNetworkPresent = false;
	preferences.begin("wifi", true);
	networkNameServer = preferences.getString("ssid", "none");    //NVS key ssid
	networkPswdServer = preferences.getString(networkNameServer.c_str(),
			"none"); //NVS key password
	//Serial.println(networkNameServer);
	//Serial.println(networkPswdServer);
	Serial.println("scan start");
	// WiFi.scanNetworks will return the number of networks found
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	delay(100);
	int16_t n = WiFi.scanNetworks();
	Serial.println("scan done");
	if (n >= 0) {
		if (n == 0) {
			Serial.println("no networks found");
		} else {
			Serial.println(String(n) + " networks found");
			if (preferences.getString(networkNameServer.c_str(), "none").compareTo(
					"none") != 0) {
				for (int i = 0; i < n && myNetworkPresent == false; ++i) {
					// Print SSID and RSSI for each network found
					if (networkNameServer.compareTo(WiFi.WiFiScanClass::SSID(i))
							== 0) {
						myNetworkPresent = true;
						Serial.println(
								"Default network found: "
										+ WiFi.WiFiScanClass::SSID(i));
					}

				}
			}
			if (!myNetworkPresent) {
				Serial.println(" Default AP is missing, searching for new one");

				for (int i = 0; i < n && myNetworkPresent == false; ++i) {
					// Print SSID and RSSI for each network found
					networkNameServer = WiFi.WiFiScanClass::SSID(i);
					networkPswdServer = preferences.getString(
							networkNameServer.c_str(), "none"); //NVS key password
					if (networkPswdServer.compareTo("none") != 0) {
						myNetworkPresent = true;
					}
					Serial.print(i + 1);
					Serial.print(": ");
					Serial.print(networkNameServer);
					Serial.print(" (");
					Serial.print(WiFi.WiFiScanClass::RSSI(i));
					Serial.print(")");
					Serial.println(
							(WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ?
									" " : "*");
					delay(10);
				}
			}

			Serial.println("");

		}
	}else{
		Serial.println("Scan Failed!! "+ String(n) );
	}

	preferences.end();
	if (!myNetworkPresent) {
		Serial.println("NO availible AP/Pass stored");

		APMode = true;
	}
}
void WifiManager::loop() {
	if (state != HaveSSIDSerial) {
		if (Serial.available() > 0) {
			networkNameServer = Serial.readString();
			if (networkNameServer.substring(0, 3).compareTo("AP:") == 0
					|| networkNameServer.substring(0, 3).compareTo("ap:")
							== 0) {
				apNameServer = networkNameServer.substring(3, 18); // ensure SSID is less than 15 char to use the SSID as key for password
				APMode = true;
				Serial.println("AP Mode ssid: " + apNameServer);
			} else {
				Serial.println("New ssid: " + networkNameServer);
				APMode = false;
			}
			state = HaveSSIDSerial;
			Serial.println("New password: ");

		}
	}
	switch (state) {
	case firstStart:
		//staticRef->printState();

		state = reconnect;
		printState();
		break;
	case InitialConnect:
		preferences.begin("wifi", false); // Note: Namespace name is limited to 15 chars
		if (preferences.getString("ssid", "none").compareTo(networkNameServer)
				!= 0) {
			Serial.println("Writing new ssid " + networkNameServer);
			preferences.putString("ssid", networkNameServer);
			delay(300);
		}
		if (preferences.getString(networkNameServer.c_str(), "none").compareTo(
				networkPswdServer) != 0) {
			Serial.println("Writing new pass ****");
			preferences.putString(networkNameServer.c_str(), networkPswdServer);
			delay(300);
		}
		preferences.end();
		state = Connected;
		printState();
		break;
	case Connected:
		//printState();

		break;
	case Disconnected:
		//printState();

		if ((millis() - timeOfLastConnect) > 5000)
			if ((millis() - timeOfLastDisconnect) > 5000) {
				Serial.println(
						"Timeouts for connection wait, reconnecting last connected: "
								+ String(timeOfLastConnect)
								+ " last disconnected: "
								+ String(timeOfLastDisconnect));
				state = reconnect;
				printState();
				connectionAttempts++;
				if (connectionAttempts > rescanIncrement) {
					connectionAttempts = 0;
					rescan();
				}
			}
		break;

	case reconnect:
		if (!APMode) {
			Serial.println("connect to WiFi network");
			//Connect to the WiFi network
			connectToWiFi(networkNameServer.c_str(), networkPswdServer.c_str());
			state = Disconnected;
		} else {
			state = Connected;
			startAP();
		}
		//printState();
		break;
	case HaveSSIDSerial:
		if (Serial.available() > 0) {
			if (!APMode)
				networkPswdServer = Serial.readString();
			else
				apPswdServer = Serial.readString();
			state = reconnect;
			printState();
		}
		break;
	}
}

void WifiManager::WiFiEvent(WiFiEvent_t event) {
	//Pass the event to the UDP Simple packet server
	switch (event) {
	case SYSTEM_EVENT_STA_GOT_IP:/**< ESP32 station got IP from connected AP */
		if (state != HaveSSIDSerial && !APMode) {
			state = InitialConnect;
			staticRef->printState();

			//When connected set
			Serial.print("\n\n\nWiFi connected! IP address: ");
			Serial.print(WiFi.localIP());
			Serial.print("\n\n\n");
			staticRef->timeOfLastConnect = millis();
		}
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED: /**< ESP32 station disconnected from AP */
		timeOfLastDisconnect = millis();
		if (state != HaveSSIDSerial && !APMode) {
			state = Disconnected;
			staticRef->printState();
			WiFi.disconnect(true);
			Serial.println("WiFi lost connection, retry " + String(
			rescanIncrement - connectionAttempts));
		}
		break;
	case SYSTEM_EVENT_WIFI_READY: /**< ESP32 WiFi ready */
		if(state == Connected){
			state = Disconnected;
			staticRef->printState();
			WiFi.disconnect(true);
			Serial.println("WiFi lost connection, retry " + String(
			rescanIncrement - connectionAttempts));
		}else{
			Serial.println("ESP32 WiFi ready ");
		}
		break;
	case SYSTEM_EVENT_SCAN_DONE: /**< ESP32 finish scanning AP */
		Serial.println("SYSTEM_EVENT_SCAN_DONE");
		break;
	case SYSTEM_EVENT_STA_START: /**< ESP32 station start */
		//Serial.println("ESP32 station start ");
		break;
	case SYSTEM_EVENT_STA_STOP: /**< ESP32 station stop */
		Serial.println("SYSTEM_EVENT_STA_STOP");
		break;
	case SYSTEM_EVENT_STA_CONNECTED: /**< ESP32 station connected to AP */
		Serial.println(" ESP32 station connected to AP ");
		break;
	case SYSTEM_EVENT_STA_AUTHMODE_CHANGE: /**< the auth mode of AP connected by ESP32 station changed */
		Serial.println("SYSTEM_EVENT_STA_AUTHMODE_CHANGE");
		break;
	case SYSTEM_EVENT_STA_LOST_IP: /**< ESP32 station lost IP and the IP is reset to 0 */
		Serial.println("SYSTEM_EVENT_STA_LOST_IP");
		break;
	case SYSTEM_EVENT_STA_WPS_ER_SUCCESS: /**< ESP32 station wps succeeds in enrollee mode */
		Serial.println("SYSTEM_EVENT_STA_WPS_ER_SUCCESS");
		break;
	case SYSTEM_EVENT_STA_WPS_ER_FAILED: /**< ESP32 station wps fails in enrollee mode */
		Serial.println("SYSTEM_EVENT_STA_WPS_ER_FAILED");
		break;
	case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT: /**< ESP32 station wps timeout in enrollee mode */
		Serial.println("SYSTEM_EVENT_STA_WPS_ER_TIMEOUT");
		break;
	case SYSTEM_EVENT_STA_WPS_ER_PIN: /**< ESP32 station wps pin code in enrollee mode */
		Serial.println("SYSTEM_EVENT_STA_WPS_ER_PIN");
		break;
	case SYSTEM_EVENT_AP_START: /**< ESP32 soft-AP start */
		Serial.println("SYSTEM_EVENT_AP_START");
		break;
	case SYSTEM_EVENT_AP_STOP: /**< ESP32 soft-AP stop */
		Serial.println("SYSTEM_EVENT_AP_STOP");
		break;
	case SYSTEM_EVENT_AP_STACONNECTED: /**< a station connected to ESP32 soft-AP */
		Serial.println("SYSTEM_EVENT_AP_STACONNECTED");
		break;
	case SYSTEM_EVENT_AP_STADISCONNECTED: /**< a station disconnected from ESP32 soft-AP */
		Serial.println("SYSTEM_EVENT_AP_STADISCONNECTED");
		break;
	case SYSTEM_EVENT_AP_PROBEREQRECVED: /**< Receive probe request packet in soft-AP interface */
		Serial.println("SYSTEM_EVENT_AP_PROBEREQRECVED");
		break;
	case SYSTEM_EVENT_AP_STA_GOT_IP6: /**< ESP32 station or ap interface v6IP addr is preferred */
		Serial.println("SYSTEM_EVENT_AP_STA_GOT_IP6");
		break;
	case SYSTEM_EVENT_ETH_START: /**< ESP32 ethernet start */
		Serial.println("SYSTEM_EVENT_ETH_START");
		break;
	case SYSTEM_EVENT_ETH_STOP: /**< ESP32 ethernet stop */
		Serial.println("SYSTEM_EVENT_ETH_STOP");
		break;
	case SYSTEM_EVENT_ETH_CONNECTED: /**< ESP32 ethernet phy link up */
		Serial.println("SYSTEM_EVENT_ETH_CONNECTED");
		break;
	case SYSTEM_EVENT_ETH_DISCONNECTED: /**< ESP32 ethernet phy link down */
		Serial.println("SYSTEM_EVENT_ETH_DISCONNECTED");
		break;
	case SYSTEM_EVENT_ETH_GOT_IP: /**< ESP32 ethernet got IP from connected AP */
		Serial.println("SYSTEM_EVENT_ETH_GOT_IP");
		break;
	case SYSTEM_EVENT_MAX:
		Serial.println("SYSTEM_EVENT_MAX");
		break;
	default:
		break;
	}
}
void WiFiEventWifiManager(WiFiEvent_t event) {
	if (staticRef != NULL)
		staticRef->WiFiEvent(event);
}

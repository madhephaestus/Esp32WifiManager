/*
 * WifiManager.cpp
 *
 *  Created on: Jun 13, 2018
 *      Author: hephaestus
 */

#include "WifiManager.h"
WifiManager *WifiManager::staticRef = NULL;
static TaskHandle_t WIFIcomplexHandlerTaskUS;
void WifiThreadLoop(void *param){
	Serial.println("WifiManager Thread Started");
	while(true){
		if(WifiManager::staticRef!=NULL)
			WifiManager::staticRef->loopThread();
		vTaskDelay(10); //sleep 10ms
	}
}


static void WiFiEventWifiManager(WiFiEvent_t event) {
	if (WifiManager::staticRef != NULL)
		WifiManager::staticRef->WiFiEvent(event);
}

WifiManager::WifiManager() {
	staticRef=this;
}

WifiManager::~WifiManager() {
	WifiManager::staticRef = NULL;
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
	case APWaitingForSTA:
		Serial.println("WifiManager.getState()=APWaitingForSTA");
		break;
	default:
		break;
	}
}
void WifiManager::setupAP() {
	APMode = true;

	startAP();
}
/**
 * Start the manager but re-scan the environment first
 */
void WifiManager::setupScan() {
	setup();
	disconnect();
}
void WifiManager::internalStart() {
	if (setupDone)
		return;
	Serial.begin(115200);
//	uint8_t mac[6];
//	char macStr[18] = { 0 };
//	esp_wifi_get_mac(WIFI_IF_STA, mac);
//	sprintf(macStr, "%02X-%02X", mac[4], mac[5]);
	String macStr = WiFi.macAddress();
	String defaultap = "esp32-" + String(macStr);

	preferences.begin("wifi", true);
	networkNameServer = preferences.getString("ssid", "none");    //NVS key ssid
	networkPswdServer = getPassword(networkNameServer); //NVS key password
	apNameServer = preferences.getString("ap-ssid", defaultap); //NVS key ssid
	apPswdServer = getPassword(apNameServer, "Wumpus3742"); //NVS key password
	preferences.end();
	staticRef = this;

	WiFi.onEvent(WiFiEventWifiManager);
	setupDone = true;
}
void WifiManager::setup() {
	internalStart();
	WiFi.mode(WIFI_MODE_AP);
	state = reconnect;
	connectionAttempts = 0;
	connectionAttempts = 1;
}

/**
 startAP
 */
void WifiManager::startAP() {
	internalStart();
	Serial.println("\n\n\nAP starting " + apNameServer);
	//Initiate connection
	WiFi.mode(WIFI_AP);
	String mac = WiFi.macAddress();
	boolean result = WiFi.softAP(apNameServer.c_str(), apPswdServer.c_str(), 6,
			0, 2, false);
	if (!result) {
		Serial.println("Failed to configure AP Mode!");
		Serial.println(String(result));
	}
	delay(100);
	IPAddress Ip(192, 168, 4, 1);    //setto IP Access Point
	IPAddress NMask(255, 255, 255, 0);
	WiFi.softAPConfig(Ip, Ip, NMask);
	timeOfLastConnect = 0;
	timeOfLastDisconnect = millis() - rescanIncrement;

	Serial.println("Mac Address: " + mac);

	preferences.begin("wifi", false); // Note: Namespace name is limited to 15 chars
	if (preferences.getString("ap-ssid", "none").compareTo(apNameServer) != 0) {
		preferences.putString("ap-ssid", apNameServer);

		delay(300);
	}
	if (getPassword(apNameServer).compareTo(apPswdServer) != 0) {
		setPassword(apNameServer, apPswdServer);
		delay(300);
	}

	preferences.end();
	Serial.println(
			"\n\nStarting AP Mode with SSID: " + apNameServer + "\npass = "
					+ apPswdServer);
	timeSinceAPStart = millis();
	state = APWaitingForSTA;
}

/**
 connectToWiFi
 */
void WifiManager::connectToWiFi(const char *ssid, const char *pwd) {

	Serial.println("Attempting to connect to WiFi network: " + String(ssid));
//Initiate connection

	WiFi.mode(WIFI_STA);
	WiFi.disconnect(true);
	delay(300);
	state = Disconnected;

	timeOfLastConnect = 0;
	timeOfLastDisconnect = millis() - rescanIncrement;
	String mac = WiFi.macAddress();
	Serial.println("Mac Address: " + mac);
	Serial.println("Waiting for WIFI connection... \n\n");
	if (state == Disconnected)
		WiFi.begin(ssid, pwd);
	else
		Serial.println("Wifi state already changed ");
}
/**
 * Update AP list
 *
 * This function will update the AP list, then reconnect
 * @return the current number of availible AP's
 * @Note this will take a few seconds and is BLOCKING during that time
 */
int WifiManager::updateApList() {
	Serial.println("scan start");
	// WiFi.scanNetworks will return the number of networks found
	WiFi.mode(WIFI_STA);
	WiFi.disconnect(true);
	delay(100);
	int16_t n = WiFi.scanComplete();
	WiFi.scanNetworks(false,
			true,
			false,
			100,
			0,
			nullptr,
			nullptr);
	state = scanRunning;
	whatToDoAfterScanning = reconnect;
	return n;
}

void WifiManager::rescan() {
	bool myNetworkPresent = false;
	preferences.begin("wifi", true);
	int16_t n = WiFi.scanComplete();
	if (n >= 0) {
		if (n == 0) {
			Serial.println("no networks found");
		} else {
			Serial.println(String(n) + " networks found");
			if (getPassword(networkNameServer).compareTo("none") != 0) {
				for (int i = 0; i < n && myNetworkPresent == false; ++i) {
					// Print SSID and RSSI for each network found
					if (networkNameServer.compareTo(WiFi.WiFiScanClass::SSID(i))
							== 0) {
						myNetworkPresent = true;
						Serial.println(
								"Default network found: "
										+ WiFi.WiFiScanClass::SSID(i));
						networkPswdServer = getPassword(networkNameServer); //NVS key password
					}

				}
			}
			if (!myNetworkPresent) {
				Serial.println(" Default AP is missing, searching for new one");
				String bestNet = "none";
				int32_t bestStren = -5000;
				for (int i = 0; i < n; ++i) {
					// Print SSID and RSSI for each network found
					String tmp = WiFi.WiFiScanClass::SSID(i);
					if (getPassword(tmp).compareTo("none") != 0) {
						myNetworkPresent = true;
						if (WiFi.WiFiScanClass::RSSI(i) > bestStren) {
							bestNet = WiFi.WiFiScanClass::SSID(i);
							bestStren = WiFi.WiFiScanClass::RSSI(i);
							networkPswdServer = getPassword(tmp); //NVS key password
						}
					}
					Serial.print(i + 1);
					Serial.print(": ");
					Serial.print(tmp);
					Serial.print(" (");
					Serial.print(WiFi.WiFiScanClass::RSSI(i));
					Serial.print(")");
					Serial.println(
							(WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ?
									" " : "*");
					delay(10);
				}
				if (myNetworkPresent) {
					networkNameServer = bestNet;
				}
			}

			Serial.println("");

		}
	} else {
		Serial.println("Scan Failed!! " + String(n));
	}

	preferences.end();
	if (!myNetworkPresent) {
		Serial.println("NO availible AP/Pass stored");

		APMode = true;
	} else
		state = reconnect;
}
void WifiManager::runSerialLoop() {
	if (state != HaveSSIDSerial) {
		if (Serial.available() > 1) {
			String tmp = Serial.readString();
			tmp.trim();
			if (tmp.substring(0, 3).compareTo("AP:") == 0
					|| tmp.substring(0, 3).compareTo("ap:") == 0) {
				String got = tmp.substring(3, 18); // ensure SSID is less than 15 char to use the SSID as key for password
				if (got.length() > 1) {
					apNameServer = got;
					state = HaveSSIDSerial;
				} else
					state = reconnect;
				APMode = true;
				Serial.println("AP Mode ssid: " + apNameServer);
			} else if ((tmp.compareTo("STA") == 0 || tmp.compareTo("sta") == 0)
					&& tmp.length() == 3) {
				Serial.println(
						"Switching to STA mode New ssid: " + networkNameServer);
				APMode = false;
				state = reconnect;
			} else if ((tmp.compareTo("ERASE") == 0
					|| tmp.compareTo("erase") == 0) && tmp.length() == 5) {
				Serial.println("Erasing all stored passwords");
				erase();
				ESP.restart();
			} else if ((tmp.compareTo("scan") == 0 || tmp.compareTo("SCAN") == 0)
					&& tmp.length() == 4) {
				Serial.println("Re-scanning and reconnecting");
				disconnect();
			} else {
				networkNameServer = tmp;
				Serial.println("New ssid: " + networkNameServer);
				APMode = false;
				state = HaveSSIDSerial;
			}
			if (state == HaveSSIDSerial)
				Serial.println("New password: ");

		}
	}
}

String WifiManager::getPasswordKey(String ssid) {
	if (ssid.length() <= PASS_LEN_KEY)
		return ssid;
	else
		return ssid.substring(0, PASS_LEN_KEY);
}
void WifiManager::setPassword(String ssid, String pass) {
	preferences.putString(getPasswordKey(ssid).c_str(), pass);
}
String WifiManager::getPassword(String ssid, String defaultPass) {
	return preferences.getString(getPasswordKey(ssid).c_str(), defaultPass);
}
/**
 * Use a thread to manage the wifi
 */
void WifiManager::startThread() {
	useThread=true;
	xTaskCreatePinnedToCore(WifiThreadLoop, "WifiManager", 8192, NULL, 2, // low priority timout thread
					&WIFIcomplexHandlerTaskUS, 1);
}
/**
 * call the loop function to update the state
 */
void WifiManager::loop(){
	if(!useThread)
		loopThread();
}
void WifiManager::loopThread() {
	long now = millis();
	runSerialLoop();
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
		if (getPassword(networkNameServer).compareTo(networkPswdServer) != 0) {
			Serial.println("Writing new pass ****");
			setPassword(networkNameServer, networkPswdServer);
			delay(300);
		}
		preferences.end();
		state = Connected;
		printState();
		break;
	case APWaitingForSTA:
		if ((timeSinceAPPrint + timeoutTime) < millis()) {
			timeSinceAPPrint = millis();
			Serial.println(
					"Waiting for client to connect to AP "
							+ String(millis() - timeSinceAPStart));
		}
		if ((timeSinceAPStart + 60000 * 5) < millis()) {
			disconnect(); // No connections after 5 minute, try connecting
		} // @suppress("No break at end of case")
		break;
	case apconnected:
		state = Connected;
		// no break
	case Connected:
		break;
	case Disconnected:
		//printState();

		if ((timeoutTime + timeOfLastConnect) < now)
			if ((timeoutTime + timeOfLastDisconnect) < now) {
				Serial.println(
						"Timeouts for connection wait, reconnecting last connected: "
								+ String(timeOfLastConnect)
								+ " last disconnected: "
								+ String(timeOfLastDisconnect) + " now "
								+ String(now));
				state = reconnect;
				printState();
				connectionAttempts++;
				if (connectionAttempts > rescanIncrement) {
					connectionAttempts = 0;
					updateApList();
					timeOfLastDisconnect = now;
					whatToDoAfterScanning = scanDone;
				}
			}
		break;
	case scanRunning:
		if (WiFi.scanComplete() != WIFI_SCAN_RUNNING) {
			state = whatToDoAfterScanning;
			Serial.println("scan DONE!");
		}
		break;
	case scanDone:
		rescan();
		state = reconnect;
		break;
	case reconnect:
		if (!APMode) {
			Serial.println("connect to WiFi network");
			//Connect to the WiFi network
			connectToWiFi(networkNameServer.c_str(), networkPswdServer.c_str());
			state = Disconnected;
		} else {
			state = APWaitingForSTA;
			startAP();
		}
		//printState();
		break;
	case HaveSSIDSerial:
		if (Serial.available() > 0) {
			if (!APMode) {
				networkPswdServer = Serial.readString();
				networkPswdServer.trim();
			} else {
				apPswdServer = Serial.readString();
				apPswdServer.trim();
			}
			state = reconnect;
			printState();
		}
		break;
	}
}

/**
 * disconnect
 * Force a disconnection and rescan of the system
 */
void WifiManager::disconnect() {
	state = Disconnected;
	printState();
	WiFi.disconnect(true);
	delay(300);
	connectionAttempts = rescanIncrement;
	timeOfLastConnect = millis() - timeoutTime - 1;
	timeOfLastDisconnect = millis() - timeoutTime - 1;
	networkNameServer = "none";
	APMode = false;

}
void WifiManager::WiFiEvent(WiFiEvent_t event) {
	if (state == HaveSSIDSerial)
		return;
	//Pass the event to the UDP Simple packet server
	switch (event) {
	case ARDUINO_EVENT_WIFI_READY:
		Serial.println("ARDUINO_EVENT_WIFI_READY");
		if (state == Connected && !APMode) {
			disconnect();
			Serial.println("WiFi lost connection, retry " + String(
			rescanIncrement - connectionAttempts));
		} else {
			Serial.println("ESP32 WiFi ready ");
		}
		break;
	case ARDUINO_EVENT_WIFI_SCAN_DONE:
		Serial.println("ARDUINO_EVENT_WIFI_SCAN_DONE");
		break;
	case ARDUINO_EVENT_WIFI_STA_START:
		Serial.println("ARDUINO_EVENT_WIFI_STA_START");
		break;
	case ARDUINO_EVENT_WIFI_STA_STOP:
		Serial.println("ARDUINO_EVENT_WIFI_STA_STOP");
		break;
	case ARDUINO_EVENT_WIFI_STA_CONNECTED:
		Serial.println("ARDUINO_EVENT_WIFI_STA_CONNECTED");
		break;
	case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
		Serial.println("ARDUINO_EVENT_WIFI_AP_STADISCONNECTED");
		// no break
	case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
		if(event==ARDUINO_EVENT_WIFI_STA_DISCONNECTED)Serial.println("ARDUINO_EVENT_WIFI_STA_DISCONNECTED");
		timeOfLastDisconnect = millis();
		if (!APMode) {
			state = Disconnected;
			printState();
			WiFi.disconnect(true);
			Serial.println("\n\nWiFi lost connection, retry " + String(
			rescanIncrement - connectionAttempts));
			timeOfLastDisconnect = millis();
		}
		break;
	case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
		Serial.println("ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE");
		break;
	case ARDUINO_EVENT_WIFI_STA_GOT_IP:
		Serial.println("ARDUINO_EVENT_WIFI_STA_GOT_IP");
		// no break
	case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
		Serial.println("ARDUINO_EVENT_WIFI_STA_GOT_IP6");
		if (!APMode) {
			state = InitialConnect;
			printState();

			//When connected set
			Serial.print(
					"\n\n\nWiFi connected! DHCP took "
							+ String(millis() - timeOfLastConnect)
							+ " IP address: ");
			Serial.print(WiFi.localIP());
			Serial.print("\n\n\n");
			timeOfLastConnect = millis();
		}
		break;
	case ARDUINO_EVENT_WIFI_STA_LOST_IP:
		Serial.println("ARDUINO_EVENT_WIFI_STA_LOST_IP");
		break;
	case ARDUINO_EVENT_WIFI_AP_START:
		Serial.println("ARDUINO_EVENT_WIFI_AP_START");
		break;
	case ARDUINO_EVENT_WIFI_AP_STOP:
		Serial.println("ARDUINO_EVENT_WIFI_AP_STOP");
		break;
	case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
		Serial.println("ARDUINO_EVENT_WIFI_AP_STACONNECTED");
		// no break
	case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
		Serial.println("ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED");
		Serial.println("\n\n ESP32 station connected to AP \n\n");
		if (APMode) {
			state = apconnected;
			timeOfLastConnect = millis();
		}
		break;
	case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
		Serial.println("ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED");
		break;
	case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
		Serial.println("ARDUINO_EVENT_WIFI_AP_GOT_IP6");
		break;
	case ARDUINO_EVENT_WIFI_FTM_REPORT:
		Serial.println("ARDUINO_EVENT_WIFI_FTM_REPORT");
		break;
	case ARDUINO_EVENT_ETH_START:
		Serial.println("ARDUINO_EVENT_ETH_START");
		break;
	case ARDUINO_EVENT_ETH_STOP:
		Serial.println("ARDUINO_EVENT_ETH_STOP");
		break;
	case ARDUINO_EVENT_ETH_CONNECTED:
		Serial.println("ARDUINO_EVENT_ETH_CONNECTED");
		break;
	case ARDUINO_EVENT_ETH_DISCONNECTED:
		Serial.println("ARDUINO_EVENT_ETH_DISCONNECTED");
		break;
	case ARDUINO_EVENT_ETH_GOT_IP:
		Serial.println("ARDUINO_EVENT_ETH_GOT_IP");
		break;
	case ARDUINO_EVENT_ETH_GOT_IP6:
		Serial.println("ARDUINO_EVENT_ETH_GOT_IP6");
		break;
	case ARDUINO_EVENT_WPS_ER_SUCCESS:
		Serial.println("ARDUINO_EVENT_WPS_ER_SUCCESS");
		break;
	case ARDUINO_EVENT_WPS_ER_FAILED:
		Serial.println("ARDUINO_EVENT_WPS_ER_FAILED");
		break;
	case ARDUINO_EVENT_WPS_ER_TIMEOUT:
		Serial.println("ARDUINO_EVENT_WPS_ER_TIMEOUT");
		break;
	case ARDUINO_EVENT_WPS_ER_PIN:
		Serial.println("ARDUINO_EVENT_WPS_ER_PIN");
		break;
	case ARDUINO_EVENT_WPS_ER_PBC_OVERLAP:
		Serial.println("ARDUINO_EVENT_WPS_ER_PBC_OVERLAP");
		break;
	case ARDUINO_EVENT_SC_SCAN_DONE:
		Serial.println("ARDUINO_EVENT_SC_SCAN_DONE");
		break;
	case ARDUINO_EVENT_SC_FOUND_CHANNEL:
		Serial.println("ARDUINO_EVENT_SC_FOUND_CHANNEL");
		break;
	case ARDUINO_EVENT_SC_GOT_SSID_PSWD:
		Serial.println("ARDUINO_EVENT_SC_GOT_SSID_PSWD");
		break;
	case ARDUINO_EVENT_SC_SEND_ACK_DONE:
		Serial.println("ARDUINO_EVENT_SC_SEND_ACK_DONE");
		break;
	case ARDUINO_EVENT_PROV_INIT:
		Serial.println("ARDUINO_EVENT_PROV_INIT");
		break;
	case ARDUINO_EVENT_PROV_DEINIT:
		Serial.println("ARDUINO_EVENT_PROV_DEINIT");
		break;
	case ARDUINO_EVENT_PROV_START:
		Serial.println("ARDUINO_EVENT_PROV_START");
		break;
	case ARDUINO_EVENT_PROV_END:
		Serial.println("ARDUINO_EVENT_PROV_END");
		break;
	case ARDUINO_EVENT_PROV_CRED_RECV:
		Serial.println("ARDUINO_EVENT_PROV_CRED_RECV");
		break;
	case ARDUINO_EVENT_PROV_CRED_FAIL:
		Serial.println("ARDUINO_EVENT_PROV_CRED_FAIL");
		break;
	case ARDUINO_EVENT_PROV_CRED_SUCCESS:
		Serial.println("ARDUINO_EVENT_PROV_CRED_SUCCESS");
		break;
	case ARDUINO_EVENT_MAX:
		Serial.println("ARDUINO_EVENT_MAX");
		break;
	default:
		Serial.println("UNKNOWN WIFI EVENT!!");
		Serial.println(String(event));
		break;
	}
}

/**
 * Check to see if the device is already in AP mode.
 */
bool WifiManager::isApMode() {
	return APMode;
}

/**
 * erase()
 * Erase all stored passwords and AP mode settings
 */
void WifiManager::erase() {
	preferences.begin("wifi", false); // Note: Namespace name is limited to 15 chars
	preferences.clear(); // erase all stored passwords
	delay(300);
	preferences.end();
}

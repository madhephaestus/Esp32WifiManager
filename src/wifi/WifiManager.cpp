/*
 * WifiManager.cpp
 *
 *  Created on: Jun 13, 2018
 *      Author: hephaestus
 */

#include <wifi/WifiManager.h>
static Preferences preferences;
void WiFiEventWifiManager(WiFiEvent_t event);
static WifiManager * staticRef = NULL;

WifiManager::WifiManager() {
	if (staticRef == NULL) {
		staticRef = this;
	}
	connectionAttempts=0;
}

WifiManager::~WifiManager() {
	// TODO Auto-generated destructor stub
}

void WifiManager::setup() {
	Serial.begin(115200);

	preferences.begin("wifi", false);
	networkNameServer = preferences.getString("ssid", "none");    //NVS key ssid
	networkPswdServer = preferences.getString(networkNameServer.c_str(),
			"none"); //NVS key password
	preferences.end();
	state = reconnect;
	WiFi.onEvent(WiFiEventWifiManager);
	connectionAttempts=1;

}
void WifiManager::connectToWiFi(const char * ssid, const char * pwd) {
	WiFi.mode(WIFI_STA);
	WiFi.disconnect(true);
	Serial.println("waiting 1 more second for WiFi to clear");
	delay(1000); // wait for WiFI stack to fully timeout
	Serial.println("Attempting to connect to WiFi network: " + String(ssid));
//Initiate connection
	WiFi.begin(ssid, pwd);

	Serial.println("Waiting for WIFI connection...");
	timeOfLastConnect = millis();
	timeOfLastDisconnect = millis();
	String mac = WiFi.macAddress();
	Serial.println("Mac Address: " + mac);
}
void WifiManager::rescan(){
	preferences.begin("wifi", false);
	networkNameServer = preferences.getString("ssid", "none");    //NVS key ssid
	networkPswdServer = preferences.getString(networkNameServer.c_str(),
			"none"); //NVS key password
	//Serial.println(networkNameServer);
	//Serial.println(networkPswdServer);
	Serial.println("scan start");
	// WiFi.scanNetworks will return the number of networks found
	int n = WiFi.scanNetworks();
	Serial.println("scan done");
	if (n == 0) {
		Serial.println("no networks found");
	} else {
		Serial.print(n);
		Serial.println(" networks found");
		bool myNetworkPresent = false;
		for (int i = 0; i < n && myNetworkPresent == false; ++i) {
			// Print SSID and RSSI for each network found
			if (networkNameServer.compareTo(WiFi.WiFiScanClass::SSID(i)) == 0) {
				myNetworkPresent = true;
			}

		}
		if (!myNetworkPresent) {
			Serial.println(" Default AP is missing, searching for new one");

			for (int i = 0; i < n && myNetworkPresent == false;
					++i) {
				// Print SSID and RSSI for each network found
				networkNameServer = WiFi.WiFiScanClass::SSID(i);
				Serial.print(i + 1);
				Serial.print(": ");
				Serial.print(networkNameServer);
				Serial.print(" (");
				Serial.print(WiFi.WiFiScanClass::RSSI(i));
				Serial.print(")");
				Serial.println(
						(WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
				delay(10);
				networkPswdServer = preferences.getString(
						networkNameServer.c_str(), "none"); //NVS key password
				if(networkPswdServer.compareTo("none") != 0){
					myNetworkPresent=true;
				}

			}
		}

		Serial.println("");
	}
	preferences.end();
}
void WifiManager::loop() {
	if (state != HaveSSIDSerial) {
		if (Serial.available() > 0) {
			networkNameServer = Serial.readString();
			state = HaveSSIDSerial;
			Serial.println("New ssid: " + networkNameServer);
			Serial.println("New password: ");

		}
	}
	switch (state) {
	case firstStart:
		state = Connected;
		//no break
	case Connected:
		break;
	case Disconnected:
		if ((millis() - timeOfLastConnect) > 5000)
			if ((millis() - timeOfLastDisconnect) > 6000){
				state = reconnect;
				connectionAttempts++;
				if(connectionAttempts>5){
					connectionAttempts=0;
					rescan();
				}
			}
		break;
	case reconnect:
		Serial.println("connect to WiFi network");
		//Connect to the WiFi network
		connectToWiFi(networkNameServer.c_str(), networkPswdServer.c_str());
		state = Disconnected;
		break;
	case HaveSSIDSerial:
		if (Serial.available() > 0) {
			networkPswdServer = Serial.readString();
			state = reconnect;
			preferences.begin("wifi", false); // Note: Namespace name is limited to 15 chars
			Serial.println("Writing new ssid " + staticRef->networkNameServer);
			preferences.putString("ssid", staticRef->networkNameServer);

			Serial.println("Writing new pass ****");
			preferences.putString(staticRef->networkNameServer.c_str(),
					staticRef->networkPswdServer);
			delay(300);
			preferences.end();
		}
		break;
	}
}
void WiFiEventWifiManager(WiFiEvent_t event) {
	//Pass the event to the UDP Simple packet server
	switch (event) {
	case SYSTEM_EVENT_STA_GOT_IP:/**< ESP32 station got IP from connected AP */
		//When connected set
		Serial.print("WiFi connected! IP address: ");
		Serial.println(WiFi.localIP());
		staticRef->state = Connected;
		preferences.begin("wifi", false); // Note: Namespace name is limited to 15 chars
		Serial.println("Writing new ssid " + staticRef->networkNameServer);
		preferences.putString("ssid", staticRef->networkNameServer);

		Serial.println("Writing new pass ****");
		preferences.putString(staticRef->networkNameServer.c_str(),
				staticRef->networkPswdServer);
		delay(300);
		preferences.end();
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED: /**< ESP32 station disconnected from AP */

		staticRef->timeOfLastDisconnect = millis();
		if (staticRef->state != HaveSSIDSerial) {
			staticRef->state = Disconnected;
			Serial.println(
					"WiFi lost connection, retry "+String(5-staticRef->connectionAttempts));
		}
		break;
	case SYSTEM_EVENT_WIFI_READY: /**< ESP32 WiFi ready */
		Serial.println("ESP32 WiFi ready ");
		break;
	case SYSTEM_EVENT_SCAN_DONE: /**< ESP32 finish scanning AP */
		Serial.println("SYSTEM_EVENT_SCAN_DONE");
		break;
	case SYSTEM_EVENT_STA_START: /**< ESP32 station start */
		Serial.println(" ESP32 station start ");
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

	}
}


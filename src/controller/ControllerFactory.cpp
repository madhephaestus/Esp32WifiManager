/*
 * ControllerFactory.c
 *
 *  Created on: Mar 6, 2018
 *      Author: hephaestus
 */

#include "EspWii.h"
#include "AbstractController.h"
#include "UdpController.h"
#include <Preferences.h>
#include <controller/LocalController.h>
#include <device/UdpNameSearch.h>
enum ControllerManager {
	Boot,
	WaitForClients,
	BeginSearch,
	WaitForSearchToFinish,
	ShutdownSearch,
	connectControllers,
	updateControllers,HaveSSIDSerial
};
static const char * ssid = NULL;
static const char * passwd = NULL;
static String * controllerNamePointer = NULL;
UDPSimplePacketComs * coms;
static Preferences preferences;
static volatile bool wifi_connected = false;
static String wifiSSID, wifiPassword;
static boolean useClient = false;
static IPAddress broadcast;
static ControllerManager state = Boot;
static long searchStartTime = 0;
static UdpNameSearch * pinger = NULL;
std::vector<IPAddress*> * FactoryAvailibleIPs;
std::vector<UdpController*> controllerList;
static volatile bool wifi_remote_Connected = false;
//when wifi connects
Accessory localController;
LocalController * local=NULL;
void wifiOnConnect() {
	Serial.println("STA Connected");
	Serial.print("STA SSID: ");
	Serial.println(WiFi.SSID());
	Serial.print("STA IPv4: ");
	Serial.println(WiFi.localIP());
	Serial.print("STA IPv6: ");
	Serial.println(WiFi.localIPv6());
	WiFi.mode(WIFI_MODE_STA);     //close AP network
}

//when wifi disconnects
void wifiOnDisconnect() {
	if (useClient) {
		Serial.println("STA Disconnected");
		delay(1000);
		WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
	}
}
void WiFiEvent(WiFiEvent_t event) {
	switch (event) {

	case SYSTEM_EVENT_AP_START:
		//can set ap hostname here
		WiFi.softAPsetHostname(ssid);
		//enable ap ipv6 here
		WiFi.softAPenableIpV6();
		break;

	case SYSTEM_EVENT_STA_START:
		//set sta hostname here
		WiFi.setHostname(ssid);
		break;
	case SYSTEM_EVENT_STA_CONNECTED:
		//enable sta ipv6 here
		WiFi.enableIpV6();
		break;
	case SYSTEM_EVENT_AP_STA_GOT_IP6:
		//both interfaces get the same event
		Serial.print("STA IPv6: ");
		Serial.println(WiFi.localIPv6());
		Serial.print("AP IPv6: ");
		Serial.println(WiFi.softAPIPv6());
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
		wifiOnConnect();
		wifi_connected = true;
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		wifi_connected = false;
		wifiOnDisconnect();
		break;
	case SYSTEM_EVENT_AP_STACONNECTED: /**< a station connected to ESP32 soft-AP */
		Serial.println("SYSTEM_EVENT_AP_STACONNECTED");
		wifi_remote_Connected=true;
		break;

	case SYSTEM_EVENT_WIFI_READY: /**< ESP32 WiFi ready */
		Serial.println("ESP32 WiFi ready ");
		break;
	case SYSTEM_EVENT_SCAN_DONE: /**< ESP32 finish scanning AP */
		Serial.println("SYSTEM_EVENT_SCAN_DONE");
		break;
	case SYSTEM_EVENT_STA_STOP: /**< ESP32 station stop */
		Serial.println("SYSTEM_EVENT_STA_STOP");
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
	case SYSTEM_EVENT_AP_STOP: /**< ESP32 soft-AP stop */
		Serial.println("SYSTEM_EVENT_AP_STOP");
		break;
	case SYSTEM_EVENT_AP_STADISCONNECTED: /**< a station disconnected from ESP32 soft-AP */
		Serial.println("SYSTEM_EVENT_AP_STADISCONNECTED");
		break;
	case SYSTEM_EVENT_AP_PROBEREQRECVED: /**< Receive probe request packet in soft-AP interface */
		Serial.println("SYSTEM_EVENT_AP_PROBEREQRECVED");
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


/**
 * Public functions
 */
void launchControllerReciver(const char * myssid, const char * mypwd,String * controllerName) {
	ssid = myssid;
	passwd = mypwd;
	controllerNamePointer=controllerName;
	Serial.begin(115200);
	Serial.println("Waiting 2 seconds for WiFi to clear");
	WiFi.disconnect(true);
	delay(1000); // wait for WiFI stack to fully timeout
	Serial.println("still waiting 1 seconds for WiFi to clear");
	delay(1000); // wait for WiFI stack to fully timeout

	WiFi.onEvent(WiFiEvent);

	preferences.begin("wifi", false);
	wifiSSID = preferences.getString("ssid", "none");           //NVS key ssid
	wifiPassword = preferences.getString("password", "none"); //NVS key password
	preferences.end();
	if (wifiSSID != "none"){
		useClient = true;
	}else{
		WiFi.mode(WIFI_MODE_APSTA);
		WiFi.softAP(ssid, passwd);
		Serial.println("AP Started");
		Serial.print("AP SSID: ");
		Serial.println(ssid);
		Serial.print("AP IPv4: ");
		Serial.println(WiFi.softAPIP());

	}
	state=Boot;

}
void loopReciver() {
	//wifiSSID, wifiPassword;
	if(state != HaveSSIDSerial){
		if(Serial.available()>0){
			wifiSSID=Serial.readString();
			state=HaveSSIDSerial;
			Serial.println("New ssid: "+wifiSSID);
			Serial.println("New password: ");

		}
	}
	switch (state) {
	case Boot:
		state = WaitForClients;
		Serial.println("Boot controller factory");
		if (useClient) {
			Serial.print("Stored SSID: ");
			Serial.println(wifiSSID);
			WiFi.disconnect(true);
			delay(1000); // wait for WiFI stack to fully timeout
			WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
		}
		break;
	case WaitForClients:
		if (wifi_remote_Connected||wifi_connected) {
			state = BeginSearch;
			if (!useClient)
				broadcast = WiFi.softAPIP();
			else{
				broadcast=WiFi.localIP();
			}
			broadcast[3] = 255;
			if(pinger==NULL){
				Serial.println("Pinnger built");
				coms = new UDPSimplePacketComs(&broadcast,useClient);
				pinger = new UdpNameSearch(coms,controllerNamePointer);
			}else{
				coms->targetDevice->fromString(broadcast.toString());
			}

		}
		//Serial.println("WaitForClients");
		break;
	case BeginSearch:
		searchStartTime = millis();
		pinger->oneShotMode();
		state = WaitForSearchToFinish;
		Serial.println("BeginSearch controller factory, Expect Timeouts "+coms->targetDevice->toString());
		break;
	case WaitForSearchToFinish:
		pinger->loop();
		if ((millis() - searchStartTime) > 10000) {
			state = ShutdownSearch;
		}
		//Serial.println("WaitForSearchToFinish");
		break;
	case ShutdownSearch:
		FactoryAvailibleIPs = getAvailibleIPs();
		if(FactoryAvailibleIPs->size()>0)
			state = connectControllers;
		else
			state = BeginSearch;
		Serial.println("ShutdownSearch controller factory");
		break;
	case connectControllers:
		if(FactoryAvailibleIPs->size()>0){
			Serial.println("Search Finished, found : "+String(FactoryAvailibleIPs->size()));
			for (int i=0;i<FactoryAvailibleIPs->size();i++) {
				IPAddress* tmp =FactoryAvailibleIPs->at(i) ;
				UDPSimplePacketComs * tmpCom = new UDPSimplePacketComs(tmp,useClient);
				Serial.print("Com's built");
				UdpController *controller =new UdpController(tmpCom);
				controllerList.push_back(controller);
				Serial.print("\r\nController built on: ");
				Serial.print(tmp[0]);
			}

			state = updateControllers;
		}else{
			state = BeginSearch;
		}
		break;
	case updateControllers:
		//Serial.println("checkForFailures");
		for (int i=0;i<controllerList.size();i++) {
			controllerList.at(i)->loop();
		}

		break;
	case HaveSSIDSerial:
		if(Serial.available()>0){
			wifiPassword=Serial.readString();
			preferences.clear();
			preferences.begin("wifi", false); // Note: Namespace name is limited to 15 chars
			Serial.println("Writing new ssid "+wifiSSID);
			preferences.putString("ssid", wifiSSID);

			Serial.println("Writing new pass *****");
			preferences.putString("password", wifiPassword);
			delay(300);
			preferences.end();
			state=Boot;
			useClient = true;

		}
		break;
	}
	if(local!=NULL){
		local->loop();
	}

}
AbstractController * getController(int id) {
	if(id==0){
		if(local==NULL)
			local= new LocalController(&localController);
		return local;
	}
	for (int i=0;i<controllerList.size();i++) {
		if(controllerList.at(i)->getId()==id)
			return controllerList.at(i);
	}
	return NULL;
}

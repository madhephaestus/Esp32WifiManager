/*
 * ControllerFactory.c
 *
 *  Created on: Mar 6, 2018
 *      Author: hephaestus
 */

#include "EspWii.h"
#include <WiiChuck.h>
#include <SimplePacketComs.h>
#include <WiFi.h>
//Are we currently connected?
//The udp library class
UDPSimplePacket * simple;
boolean firstStart=true;
boolean connected = false;
long timeOfLastDisconnect=0;
long timeOfLastConnect=0;
const  char * networkNameServer;
const char * networkPswdServer;
void WiFiEventServer(WiFiEvent_t event) ;
void connectToWiFi(const char * ssid, const char * pwd) ;
/**
 * Public functions
 */
void launchControllerServer(const char * myssid, const char * mypwd,PacketEventAbstract * eventImplementation ) {
	Serial.begin(115200);
	Serial.println("Waiting 10 seconds for WiFi to clear");
	WiFi.disconnect(true);
	delay(5000);// wait for WiFI stack to fully timeout
	Serial.println("still waiting 5 more seconds for WiFi to clear");
	delay(5000);// wait for WiFI stack to fully timeout
	networkNameServer=myssid;
	networkPswdServer=mypwd;
	// put your setup code here: to run once:

	simple = new UDPSimplePacket();
	//pinMode(23, INPUT);           // set pin to input
	//Connect to the WiFi network
	connectToWiFi(myssid, mypwd);
	//classic->enableEncryption(true);
	//simple->attach(new WiiClassicServerEvent(&classic,id));
	addServer( eventImplementation );
	//delay(1000);
	WiFi.onEvent(WiFiEventServer);

}
void addServer(PacketEventAbstract * eventImplementation ){
	simple->attach(eventImplementation);
}

void loopServer() {
	if(connected){
		if(firstStart){
			firstStart=false;
			//classic.begin();
		}
		simple->server();
	}else{
		if((millis()-timeOfLastConnect)>10000){
			if((millis()-timeOfLastDisconnect)>11000){
				Serial.println("Re-connect to WiFi network");
				//Connect to the WiFi network
				connectToWiFi(networkNameServer, networkPswdServer);
			}
		}
	}

}


void connectToWiFi(const char * ssid, const char * pwd) {
	Serial.println("Attempting to connect to WiFi network: " + String(ssid));

//Initiate connection
	WiFi.begin(ssid, pwd);

	Serial.println("Waiting for WIFI connection...");
	timeOfLastConnect=millis();
	timeOfLastDisconnect=millis();
}
void WiFiEventServer(WiFiEvent_t event) {
	//Pass the event to the UDP Simple packet server
	simple->WiFiEvent(event);
	switch (event) {
	case SYSTEM_EVENT_STA_GOT_IP:/**< ESP32 station got IP from connected AP */
		//When connected set
		Serial.print("WiFi connected! IP address: ");
		Serial.println(WiFi.localIP());
		connected = true;
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED: /**< ESP32 station disconnected from AP */
		Serial.println("WiFi lost connection, waiting 10 seconds to reconnect");
		timeOfLastDisconnect=millis();
		connected = false;
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



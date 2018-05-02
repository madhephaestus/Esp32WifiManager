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
#include <Preferences.h>

//Are we currently connected?
//The udp library class
UDPSimplePacket * simple;
//boolean connected = false;
long timeOfLastDisconnect=0;
long timeOfLastConnect=0;
static String networkNameServer;
static String networkPswdServer;
void WiFiEventServer(WiFiEvent_t event) ;
void connectToWiFi(const char * ssid, const char * pwd) ;
static Preferences preferences;
static enum connectionState {firstStart,Disconnected,Connected,HaveSSIDSerial, reconnect} state=firstStart ;

class NameCheckerServer: public PacketEventAbstract {
	String * namePointer;
public:
	// Packet ID needs to be set
	NameCheckerServer(String *robot):
		PacketEventAbstract(1917)// Address of this event
	{
		namePointer=robot;
	}
	//User function to be called when a packet comes in
	// Buffer contains data from the packet coming in at the start of the function
	// User data is written into the buffer to send it back
	void event(float * buffer){
		if(namePointer==NULL){
			return;
		}
		char * bytes = (char *)buffer;

		for (int i=0;i<namePointer->length();i++){
			if(bytes[i]=='*'){
				break;// if we match up to a wildcard, assume match
			}
			if(bytes[i]!=namePointer->charAt(i)&&
			   bytes[i]!=0xFF	){
				Serial.print("\r\nFailed name check "+namePointer[0]+" got: ");
				for (int j=0;j<namePointer->length();j++){
					Serial.print(bytes[j]);
				}
				noResponse=true;//Name check failed
				return;
			}
		}
		int i;
		for ( i=0;i<namePointer->length()&& i<60;i++){
			bytes[i]=namePointer->charAt(i);
		}for ( i=namePointer->length();i<60;i++){
			bytes[i]=0;
		}
		Serial.print("\r\nSuccess name check "+namePointer[0]);
	}
};
void launchControllerServer() {
	Serial.begin(115200);

	preferences.begin("wifi", false);
	networkNameServer = preferences.getString("ssid", "none");           //NVS key ssid
	networkPswdServer = preferences.getString("password", "none"); //NVS key password
	preferences.end();
	// put your setup code here: to run once:

	simple = new UDPSimplePacket();
	//Connect to the WiFi network
	state=reconnect;
	//delay(1000);
	WiFi.onEvent(WiFiEventServer);
}

void setNameUdpDevice(String *robot ){
	addServer( new NameCheckerServer(robot) );
	Serial.println("Setting controller name to: "+robot[0]);
}
void addServer(PacketEventAbstract * eventImplementation ){
	simple->attach(eventImplementation);
}

void loopServer() {
	if(state != HaveSSIDSerial){
		if(Serial.available()>0){
			networkNameServer=Serial.readString();
			state=HaveSSIDSerial;
			Serial.println("New ssid: "+networkNameServer);
			Serial.println("New password: ");

		}
	}
	switch(state){
	case firstStart:
		state=Connected;
		//no break
	case Connected:
		simple->server();
		break;
	case Disconnected:
		if((millis()-timeOfLastConnect)>5000)
			if((millis()-timeOfLastDisconnect)>6000)
				state=reconnect;
		break;
	case reconnect:
		Serial.println("connect to WiFi network");
		//Connect to the WiFi network
		connectToWiFi(networkNameServer.c_str(), networkPswdServer.c_str());
		state=Disconnected;
		break;
	case HaveSSIDSerial:
		if(Serial.available()>0){
			networkPswdServer=Serial.readString();
			preferences.clear();
			preferences.begin("wifi", false); // Note: Namespace name is limited to 15 chars
			Serial.println("Writing new ssid "+networkNameServer);
			preferences.putString("ssid", networkNameServer);

			Serial.println("Writing new pass "+networkPswdServer);
			preferences.putString("password", networkPswdServer);
			delay(300);
			preferences.end();
			state=reconnect;
		}
		break;
	}

}


void connectToWiFi(const char * ssid, const char * pwd) {
	WiFi.disconnect(true);
	Serial.println("waiting 1 more second for WiFi to clear");
	delay(1000);// wait for WiFI stack to fully timeout
	Serial.println("Attempting to connect to WiFi network: " + String(ssid));
//Initiate connection
	WiFi.begin(ssid, pwd);

	Serial.println("Waiting for WIFI connection...");
	timeOfLastConnect=millis();
	timeOfLastDisconnect=millis();
	String mac = WiFi.macAddress();
	Serial.println("Mac Address: "+mac);
}
void WiFiEventServer(WiFiEvent_t event) {
	//Pass the event to the UDP Simple packet server
	simple->WiFiEvent(event);
	switch (event) {
	case SYSTEM_EVENT_STA_GOT_IP:/**< ESP32 station got IP from connected AP */
		//When connected set
		Serial.print("WiFi connected! IP address: ");
		Serial.println(WiFi.localIP());
		state=Connected;
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED: /**< ESP32 station disconnected from AP */
		Serial.println("WiFi lost connection, waiting 10 seconds to reconnect");
		timeOfLastDisconnect=millis();
		state=Disconnected;
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



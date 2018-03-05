#include <UDPSimplePacket.h>

UDPSimplePacket::UDPSimplePacket(WiFiUDP * incomingUdp) {
	udp = incomingUdp;
	connected = false;
}
UDPSimplePacket::UDPSimplePacket() {
	udp = new WiFiUDP();
	connected = false;
}

/**
 * Non blocking function to check if a packet is availible from the physical layer
 */
bool UDPSimplePacket::isPacketAvailible() {
	if (!connected) {
		return false;
	}
	udp->parsePacket();

	if (udp->available() > 0) {
		//printf("\nReceived packet");

		return true;
	}
	return false;
}
/**
 * User function that fills the buffer from the data in the physical layer
 * this data should already be framed, checksummed and validated as a valid packet
 */
int32_t UDPSimplePacket::getPacket(uint8_t * buffer, uint32_t numberOfBytes) {
	if (!connected) {
		return 0;
	}
	return udp->read(buffer, numberOfBytes);
}
/**
 * User function that sends the buffer to the physical layer
 * this data should already be framed, checksummed and validated as a valid packet
 */
int32_t UDPSimplePacket::sendPacket(uint8_t * buffer, uint32_t numberOfBytes) {
	if (!connected) {
		return 0;
	}
	if (!udp->beginPacket()) {
		return -1;
	}
	int ret = udp->write(buffer, numberOfBytes);
	if (udp->endPacket())
		return ret;
	return -2;
}

//wifi event handler
void UDPSimplePacket::WiFiEvent(WiFiEvent_t event) {
	switch (event) {
	case SYSTEM_EVENT_STA_GOT_IP:
		//When connected set
		Serial.print("WiFi connected! IP address: ");
		Serial.println(WiFi.localIP());
		//initializes the UDP state
		//This initializes the transfer buffer
		udp->begin(WiFi.localIP(), UDESIMPLEPORT);
		connected = true;
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		Serial.println("WiFi lost connection");
		connected = false;
		break;
	case SYSTEM_EVENT_WIFI_READY: /**< ESP32 WiFi ready */
		Serial.println("");
		break;
	case SYSTEM_EVENT_SCAN_DONE: /**< ESP32 finish scanning AP */
		Serial.println("SYSTEM_EVENT_SCAN_DONE");
		break;
	case SYSTEM_EVENT_STA_START: /**< ESP32 station start */
		Serial.println("SYSTEM_EVENT_STA_START");
		break;
	case SYSTEM_EVENT_STA_STOP: /**< ESP32 station stop */
		Serial.println("SYSTEM_EVENT_STA_STOP");
		break;
	case SYSTEM_EVENT_STA_CONNECTED: /**< ESP32 station connected to AP */
		Serial.println("SYSTEM_EVENT_STA_CONNECTED");
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


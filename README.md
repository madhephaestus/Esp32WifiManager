# Esp32WifiManager 
A framework for controlling the ESP 32's wifi stack

# Connect to Wifi Network
To connect to a network named "mine" with a password "pass" do the following:

Type:
```
mine
```
and wait for this to print:
```
ssid: mineWifiManager.getState()=HaveSSIDSerial
New password: 
```

Type:
```
pass
```


# AP mode 

When no network is present, the WiFiManager will fail over to AP mode where it will host its own AP. The AP ssid:pass is set via the serial port. To indicate AP SSID setting into the serial window then hit enter. Here is what setting the procedure looks like for an AP named "mine" with password "pass"

Type:
```
AP:mine
```
and wait for this to print:
```
AP Mode ssid: mineWifiManager.getState()=HaveSSIDSerial
New password: 
```

Type:
```
pass
```
And in the terminal:
```
Send to /dev/ttyUSB0: "pass"
WifiManager.getState()=reconnect



AP starting mine
SYSTEM_EVENT_STA_STOP
SYSTEM_EVENT_STA_STOP
ESP32 WiFi ready 
SYSTEM_EVENT_STA_STOP
SYSTEM_EVENT_AP_START
SYSTEM_EVENT_AP_START
Mac Address: 24:0A:C4:1D:47:84
Writing new AP ssid mine
Starting AP With pass pass
```
# Switch to AP mode withouth changing settings

Type 
```
ap:
```
in the terminal. The device will host the AP configuration it has in memory.

# Switching to Station mode

Type:
```
STA
```
in the terminal. It will connect to the default AP and rescan if the default is not availible. 

# Erase all stored passwords

type:
```
ERASE
```
in the terminal. THis will erase all the stored passwords and reboot the ESP32.

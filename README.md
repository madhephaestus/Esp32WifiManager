# EspWii
A framework for controlling one ESP from another using various Wii accessories. 

# Known Commands

## Get Name
| |ID | bytes |
|--- |--- | --- |
|Number of Bytes |4 | 60 |
| Contents downstream |1917 | String of Name or "*" |
| Contents upstream |1917 | String of Name|

This checks the device to see if the name of the device matched a given name. If the name matches a packet is returned with the device name in its contents. If the name does not match, no packet is returned. 

If the name requests ends with a \* charrecter then it wil be treated as a wildcard. Matches up to the \* will be returned. Name requests with just \* will always return the name of the device. 

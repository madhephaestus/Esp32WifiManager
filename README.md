# EspWii
A framework for controlling one ESP from another using various Wii accessories. 

# Known Commands

## Get Name (All UDP devices)
| |ID | bytes |
|--- |--- | --- |
|Number of Bytes |4 | 60 |
| Contents downstream |1917 | String of Name or \* |
|Number of Bytes |4 | 60 |
| Contents upstream |1917 | String of Name|

This checks the device to see if the name of the device matched a given name. If the name matches a packet is returned with the device name in its contents. If the name does not match, no packet is returned. 

If the name requests ends with a \* charrecter then it wil be treated as a wildcard. Matches up to the \* will be returned. Name requests with just \* will always return the name of the device. 

## Warehouse Robot

### Get Status
| |ID | byte |
|--- |--- | --- |
|Number of Bytes |4 | 0 |
| Contents downstream |2012 | --- |
|Number of Bytes |4 | 1 |
| Contents upstream |2012 | status value|

Request the status of the robot. The byte value represents the robot state.

| State description | value |
|--- |--- |
| Ready for new task | 0 |
| Heading to pickup | 1 |
| Picking up | 2 |
| Heading to Dropoff | 3 |
| Dropping off | 4 |
| Heading to safe zone| 5 |
| Fault: failed pickup| 6 |
| Fault: failed dropoff| 7 |
| Fault: excessive load| 8 |
| Fault: obstructed path| 9 |
| Fault: E Stop pressed | 10 |

### Clear Faults 

| |ID | byte |
|--- |--- | --- |
|Number of Bytes |4 | 0 |
| Contents downstream |1871 | --- |
|Number of Bytes |4 | 0 |
| Contents upstream |1871 | ---|

Clear all faults on the robot. No data is transmitted, but the state of the robot should be effected. 





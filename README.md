# EspWii
A framework for controlling one ESP from another using various Wii accessories. 

# Known Commands

## Get Name (All UDP devices)
| |ID | bytes |
|--- |--- | --- |
| downstream  Bytes |4 | 60 |
| Contents downstream |1776 | String of Name or \* |
| upstream  Bytes |4 | 60 |
| Contents upstream |1776 | String of Name|

This checks the device to see if the name of the device matched a given name. If the name matches a packet is returned with the device name in its contents. If the name does not match, no packet is returned. 

If the name requests ends with a \* charrecter then it wil be treated as a wildcard. Matches up to the \* will be returned. Name requests with just \* will always return the name of the device. 

## Warehouse Robot

### Get Status
| |ID | byte |
|--- |--- | --- |
| downstream Bytes |4 | 0 |
| Contents downstream |2012 | --- |
| upstream Bytes |4 | 1 |
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
| downstream Bytes |4 | 0 |
| Contents downstream |1871 | --- |
| upstream Bytes |4 | 0 |
| Contents upstream |1871 | ---|

Clear all faults on the robot. No data is transmitted, but the state of the robot should be effected. 

### Pick Order

| |ID | float |float |float |float |float |float |
|--- |--- | --- |--- | --- |--- | --- | --- |
| downstream Bytes |4 | 4 | 4 | 4 | 4 | 4 | 4 |
| Contents downstream |1936 | pickup area | pickup x | pickup z | drop off area | drop off  x | drop off  z |
| upstream Bytes |4 | 0 |
| Contents upstream |1936 | ---|

An order is placed with the robot. The first 3 values are the pickup location, followed by the x and Z values in MM of the shelf where the pallet is located. The second 3 values are the drop off location, followed by the X and Z values in MM of the shelf where the pallet is to be deposited. 

### Get Location 
| |ID | float |float |float |float |float |float |float |float |
|--- |--- | --- |--- | --- |--- | --- | --- |--- | --- | 
| downstream Bytes |4 | 0 |
| Contents downstream |1994 | --- |
| upstream Bytes |4 | 4 | 4 | 4 | 4 | 4 | 4 |4 | 4 |
| Contents upstream |1994 | X Location | Y Location | Z Location | azimuth | elevation | tilt | bounding cylinder radius | bounding cylinder height | 

Request for the position and orentation of the robot. Location is in MM from the loading point in location 0 to the center bottom of the bounding cylinder of the robot. The size of the robot is reported in MM measured in positive values from center bottom of the bounding cylinder that envelops the robot. The center is defined as the turning center of the robot. The radius is the distance to the furthest point on the robot from the turning center. Maximum radius is 100mm. Direction of travil is +X in the robots coordinate frame.

### Direct Drive
| |ID | float |float |float |float |float |float |float |float |
|--- |--- | --- |--- | --- |--- | --- | --- |--- |--- |
| downstream Bytes |4 | 4 | 4 | 4 | 4 | 4 | 4 |4 |4 |
| Contents downstream |1786  | delta X Location | delta Y Location |delta  Z Location | delta azimuth | delta elevation | delta tilt | # Miliseconds this update should take | Session ID|
| upstream Bytes |4 | 4 |
| Contents upstream |1786  | value from 0-1 to indicate progress on current session  |

This is a command to drive a robot directly. The values represent a relative motion from current location. +X is forward for the robot.Angle values are in degrees and translation values are in Milimeters.  Azimuth values range from -180 to 180 with 0 along the robots X dimention. Elevation values are rotation values from -90 to 90 about the Y dimention with 0 being along the x dimenttion. Tilt values are rotations from -180 to 180 about the X dimention with 0 being aligned with the X-Y plane. 

The session ID is a random integer from 0 - 100000 indicating the current drive command. It will change when the target is updateted. It is used to differentiate between new commands. 

The command should return the current progress on the current command ID. 

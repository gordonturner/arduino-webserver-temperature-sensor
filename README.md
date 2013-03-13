Arduino Web Server Temperature Sensor
=====================================

A web server that will read the values of a 1-Wire temperature sensor and output 
XML with temperature in Celcius and Fahrenheit.

- Adapted by Gordon Turner from code by David A. Mellis and Tom Igoe.

- Ethernet shield attached to pins 10, 11, 12, 13
- 1-Wire temperature sensor attached to digital pin 3.
 
- Requires 1-wire Ardunio Library and Dallas Temperature Control Arduino Library.
- Included in Libraries folder or online:
    http://www.pjrc.com/teensy/arduino_libraries/OneWire.zip
    http://milesburton.com/Dallas_Temperature_Control_Library#Latest
    
- Sample output XML:
```XML
<?xml version=\"1.0\"?>
<xml>
<temperature>
<celcius>0</celcius>
<fahrenheit>32</fahrenheit>
</temperature>
</xml>
```
- Sample output XML with error:
```XML
<?xml version=\"1.0\"?>
<xml>
<temperature>
ERROR
</temperature>
</xml>
```
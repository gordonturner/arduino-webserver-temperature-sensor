Arduino Web Server Temperature Sensor
=====================================

- Processing code for Ardunio to create a web server that reads from a 1-wire temperature sensor and outputs xml.

- Adapted by Gordon Turner from code by David A. Mellis and Tom Igoe.
 
- A web server that will read the values of a 1-Wire temperature sensor and output XML with temperature in Celcius and Fahrenheit.

- Ethernet shield attached to pins 10, 11, 12, 13
- 1-Wire temperature sensor attached to digital pin 3.

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
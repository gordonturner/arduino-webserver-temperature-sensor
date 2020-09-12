# Arduino Webserver Temperature Sensor

Arduino web server using an Ethernet Shield and 1-Wire temperature sensor to output XML, JSON, JSONP or Prometheus (prom format) response.

- Adapted by Gordon Turner from code by David A. Mellis and Tom Igoe.

- Ethernet shield attached to pins 10, 11, 12, 13
- 1-Wire temperature sensor attached to digital pin 3.
 
- Requires 1-wire Ardunio Library and Dallas Temperature Control Arduino Library.
- Included in Libraries folder or online:
    http://www.pjrc.com/teensy/arduino_libraries/OneWire.zip
    http://milesburton.com/Dallas_Temperature_Control_Library#Latest

- Send format parameter for XML, JSON, JSONP (with extra callback parameter) or Prometheus prom format.
- Defaults to JSON.


## XML

- Sample XML request URL:

    http://192.168.2.70/?format=XML


- Sample XML response:

```
XML
<?xml version=\"1.0\"?>
<xml>
<temperature>
<celcius>0</celcius>
<fahrenheit>32</fahrenheit>
</temperature>
</xml>
```


- Sample XML response with error:

```
XML
<?xml version=\"1.0\"?>
<xml>
<temperature>
ERROR
</temperature>
</xml>
```


## JSON

- Sample JSON request URL:

    http://192.168.2.70/?format=JSON
    
    
- Sample JSON response:

```
{
  "temperature": {
    "celcius": 12.50,
    "fahrenheit": 54.50
  }
}
```


- Sample JSON response with error:

```
{
  "error": "error"
}
```


## JSONP

- Sample JSONP request URL:

    http://192.168.2.70/?format=JSONP&callback=MyCallback
   
    
- Sample JSONP response:

```
MyCallback({
  "temperature": {
    "celcius": 12.50,
    "fahrenheit": 54.50
  }
});
```


- Sample JSON response with error:

```
MyCallback({
  "error": "error"
});
```


## Prometheus

http://192.168.2.70/metric

```
arduino_temperature_probe 20.00
```

NOTE: Error state will return a HTTP 500.

NOTE: As per Prometheus best practices, only Celcius is returned.

Config for `prometheus.yml`:

```
  - job_name: 'External Ambient Temperature Monitor'
    scrape_interval: 5s
    static_configs:
      - targets: ['192.168.2.70:80']
```
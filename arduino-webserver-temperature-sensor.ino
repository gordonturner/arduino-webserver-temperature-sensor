/*
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
 
  <?xml version=\"1.0\"?>
  <xml>
  <temperature>
  <celcius>0</celcius>
  <fahrenheit>32</fahrenheit>
  </temperature>
  </xml>

  - Sample output XML with error:
  
  <?xml version=\"1.0\"?>
  <xml>
  <temperature>
  ERROR
  </temperature>
  </xml>
  
*/

#include <SPI.h>
#include <Ethernet.h>

#include <OneWire.h>
#include <DallasTemperature.h>


// 1-Wire data wire is attached to ditigal pin 3.
#define ONE_WIRE_BUS 3

// Intialize an 1-Wire instance.
OneWire oneWire(ONE_WIRE_BUS);

// Pass 1-Wire reference to Dallas Temperature library.
DallasTemperature sensors(&oneWire);

// 1-Wire sensor address, use OneWireAddressFinder to find.
DeviceAddress outsideThermometer = { 0x28, 0x85, 0x95, 0x3A, 0x04, 0x00, 0x00, 0xB7 };

// Ethernet Shield MAC address, use value on back of shield.
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x10, 0x5A };

// IP address, will depend on your local network.
IPAddress ip(192,168,2,70);

// Initialize the Ethernet server library, use port 80 for HTTP.
EthernetServer server(80);

// String for reading from client
String request = String(100);
String parsedRequest = "";



// NOTE: Serial debugging code has been disabled.

void setup() 
{
  // Open Serial communications and wait for port to open.
  Serial.begin(9600);
 
   // Start the sensor library.
  sensors.begin();
  
  if( !sensors.isConnected(outsideThermometer) )
  {
    Serial.println("Error, no sensors found.");
  }
  else
  {
    Serial.println("Found sensor, setting up.");
  }
  
  // Set the resolution to 10 bit.
  sensors.setResolution(outsideThermometer, 10);
  
  Serial.println("Setting up Ethernet");

  // Start the Ethernet connection and the server.
  Ethernet.begin(mac, ip);
  server.begin();
  
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() 
{
  // Listen for incoming clients.
  EthernetClient client = server.available();
  
  if (client) 
  {
    Serial.println("Client connected");
  
    // An http request ends with a blank line.
    boolean currentLineIsBlank = true;
    
    while (client.connected()) 
    {
      if (client.available()) 
      {
        char c = client.read();
        
        // Read http request.
        if (request.length() < 100) 
        {
          request += c; 
        } 
      
        // If you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply.
        if (c == '\n' && currentLineIsBlank) 
        {
          Serial.println("Finished reading request.");
          Serial.println("http request: '" + request + "'");
          
          // Response looks like:
          // GET /?format=JSONP HTTP/1.1
          // Host: 192.168.2.70
          // User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X

          if (request.startsWith("GET /?format=")) 
          {
            parsedRequest = request.substring(request.indexOf('format=')+1, request.indexOf("HTTP")-1);
          }
          else if (request.startsWith("GET /metrics"))
          {
            parsedRequest = "prometheus";
          }

          Serial.println("Parsed request: '" + parsedRequest + "'");
          Serial.println("Reading sensor."); 
          
          float celsius;
          float fahrenheit;
          String error = "";
          
          if( sensors.isConnected(outsideThermometer) )
          {
            sensors.requestTemperatures();
            celsius = sensors.getTempC(outsideThermometer);
            fahrenheit = sensors.getTempF(outsideThermometer);
          
            Serial.print("C: ");
            Serial.println(celsius);
            Serial.print("F: ");
            Serial.println(fahrenheit);
          }
          else
          {
            error = "Error reading sensors";
            Serial.println(error);
          }
          
          
          if( parsedRequest == "XML" )
          {
             sendXmlResponse(client, celsius, fahrenheit, error);
          }
          else if( parsedRequest.startsWith("JSONP") )
          {
            // Parse callback arguement.
            String callback = parsedRequest.substring(parsedRequest.indexOf('callback=')+1, parsedRequest.length());
            callback = callback.substring(0, callback.indexOf('&'));
            
            Serial.println("Using callback: " + callback);
            
            sendJsonpResponse(client, celsius, fahrenheit, error, callback);
          }
          else if( parsedRequest == "prometheus" )
          {
            sendPrometheusResponse(client, celsius, error);
          }
          else
          {
            // Default to JSON.
            sendJsonResponse(client, celsius, fahrenheit, error);
          }
          
          
          break;
        }
        
        if (c == '\n') 
        {
          // Character is a new line.
          currentLineIsBlank = true;
        } 
        else if (c != '\r') 
        {
          // Character is not a new line or a carriage return.
          currentLineIsBlank = false;
        }
        
      }
      
    }
    
    // Give the web browser time to receive the data.
    delay(1);
    
    // Close the connection:
    client.stop();
    
    Serial.println("Client disonnected");
  }
  
  // Reset the request.
  request = "";
  parsedRequest = "";
}


/*
 *
 */
void sendXmlResponse(EthernetClient client, float celsius, float fahrenheit, String error)
{
  if(error == "")
  {
    Serial.println("Sending XML response.");
    
    // Send a standard http response header.
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/xml");
    client.println("Connnection: close");
    client.println();
    
    // Send XML body.
    client.println("<?xml version=\"1.0\"?>");
    client.println("<xml>");
    client.print("<temperature>");
    client.print("<celcius>");
    client.print(celsius);
    client.print("</celcius>");
    client.print("<fahrenheit>");
    client.print(fahrenheit);
    client.print("</fahrenheit>");
    client.println("</temperature>");            
    client.println("</xml>");
  }
  else
  {
    Serial.println("Sending XML error response.");
    
    // Send a standard http response header.
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/xml");
    client.println("Connnection: close");
    client.println();
    
    // Send XML body.
    client.println("<?xml version=\"1.0\"?>");
    client.println("<xml>");  
    client.print("<temperature>");
    client.print(error);
    client.println("</temperature>");        
    client.println("</xml>");            
  }
}


/*
 *
 */
void sendJsonpResponse(EthernetClient client, float celsius, float fahrenheit, String error, String callback)
{
  
  if(error == "")
  {
    Serial.println("Sending JSONP response.");
    
    // Send a standard http response header.
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connnection: close");
    client.println();
    
    // Send JSONP body.
    client.println(callback + "({");
    client.println("  \"temperature\": {");
    client.print("    \"celcius\": ");
    client.print(celsius);
    client.println(",");
    client.print("    \"fahrenheit\": ");
    client.println(fahrenheit);
    client.println("  }");
    client.println("});");
  }
  else
  {
    Serial.println("Sending JSONP error response.");
    
    // Send a standard http response header.
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connnection: close");
    client.println();
    
    // Send JSONP body.
    client.println(callback + "({");
    client.println("  \"error\": \"" + error + "\"");
    client.println("})");     
  }
}


/*
 *
 */
void sendPrometheusResponse(EthernetClient client, float celsius, String error)
{
  if(error == "")
  {
    Serial.println("Sending Prometheus response.");
    
    // Send a standard http response header.
    client.println("HTTP/1.1 200 OK");

    // Content-Type from https://github.com/siimon/prom-client/blob/master/lib/registry.js
    // 'text/plain; version=0.0.4; charset=utf-8'
    client.println("Content-Type: text/plain; version=0.0.4; charset=utf-8");
    client.println("Connnection: close");
    client.println();
    
    // Send Prometheus body.
    client.print("arduino_temperature_probe ");
    client.print(celsius);
  }
  else
  {
    Serial.println("Sending Prometheus error response.");
    
    // Send a standard http response header.
    client.println("HTTP/1.1 500 Internal Server Error");
    client.println("Connnection: close");
    client.println();   
  }
}


/*
 *
 */
void sendJsonResponse(EthernetClient client, float celsius, float fahrenheit, String error)
{
  if(error == "")
  {
    Serial.println("Sending JSON response.");
    
    // Send a standard http response header.
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connnection: close");
    client.println();
    
    // Send JSON body.
    client.println("{");
    client.println("  \"temperature\": {");
    client.print("    \"celcius\": ");
    client.print(celsius);
    client.println(",");
    client.print("    \"fahrenheit\": ");
    client.println(fahrenheit);
    client.println("  }");
    client.println("}");
  }
  else
  {
    Serial.println("Sending JSON error response.");
    
    // Send a standard http response header.
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connnection: close");
    client.println();
    
    // Send JSON body.
    client.println("{");
    client.println("  \"error\": \"" + error + "\"");        
    client.println("}");            
  }
}

#include <stdio.h>
#include <string.h>
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

IPAddress ip(192, 168, 0, 180);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 0, 1);

#define led1 14

const char* ssid     = "embarcados";
const char* password = "embarcados";

uint8_t rec_value= 0;
String inChar;
int i;
int rele = 23;

const int oneWireBus = 4;  // GPIO where the DS18B20 is connected to
OneWire oneWire(oneWireBus); // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature sensor 

WiFiServer server(80);

void setup()
{
    Serial.begin(115200);
    pinMode(5, OUTPUT); 
    pinMode(rele, OUTPUT);

 
    pinMode(led1, OUTPUT);
//    digitalWrite(14,0); //inverter valor rele
    
    sensors.begin();
    
    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.config(ip, dns, gateway, subnet);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();

}

int value = 0;

void loop(){
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
      
      int luminosidade = 100;



      
        String request = client.readStringUntil('\r');
        Serial.println(request);
//        client.flush();
         
         // analisar os dados que chegaram
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println(); 
          
        if (request.indexOf(F("led1")) != -1) {
          digitalWrite(led1, !digitalRead(led1));
          Serial.println("LED 1 Chegou");     
          client.println("LED1");   
        } 


          client.println("Oi");


               
//       while (client.available()) {
//      // byte by byte is not very efficient
//      client.read();
//      }



    


    // The client will actually be *flushed* then disconnected
    // when the function returns and 'client' object is destroyed (out-of-scope)
    // flush = ensure written data are received by the other side
    Serial.println(F("Disconnecting from client"));
  
    }

//  digitalWrite(rele, HIGH);
//  delay(5000);
//  Serial.println("LIGOU RELE");
//  digitalWrite(rele, LOW);
//  delay(5000);
//  Serial.println("DESLIGOU RELE");
//
//
//  sensors.requestTemperatures(); 
//  float temperatureC = sensors.getTempCByIndex(0);
//  float temperatureF = sensors.getTempFByIndex(0);
//  Serial.print(temperatureC);
//  Serial.println("ºC");
//  Serial.print(temperatureF);
//  Serial.println("ºF");
//  delay(5000);
  
}


#include <FS.h>
//#include "Free_Fonts.h" 
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <stdio.h>
#include <string.h>
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <RTClib.h>
#include "menu1.h"
#include "menu2.h"
#include "menu3.h"
#include "menu4.h"
#include "menu5.h"

#include <TJpg_Decoder.h>

#define USE_DMA


#ifdef USE_DMA
  uint16_t  dmaBuffer1[16*16]; // Toggle buffer for 16*16 MCU block, 512bytes
  uint16_t  dmaBuffer2[16*16]; // Toggle buffer for 16*16 MCU block, 512bytes
  uint16_t* dmaBufferPtr = dmaBuffer1;
  bool dmaBufferSel = 0;
#endif



RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String timeChar, timeChar2;
char dateChar[50];
char temperatureChar[10];
String dateString;
int minuteNow=0, minuteNow2=0, horaNow=0;
int minutePrevious=0;

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

IPAddress ip(192, 168, 0, 180);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 0, 1);

// This is the file name used to store the touch coordinate
// calibration data. Cahnge the name to start a new calibration.
#define CALIBRATION_FILE "/TouchCalData3"

// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
#define REPEAT_CAL false

#define led1 14 

boolean SwitchOn = false;

const char* ssid     = "embarcados";
const char* password = "embarcados";



uint8_t rec_value= 0;
String inChar;
int i, hs, mi, quantidade;
int releRes = 25, releAgua= 33;


const int oneWireBus = 27;  // GPIO where the DS18B20 is connected to
OneWire oneWire(oneWireBus); // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature sensor 

WiFiServer server(80);


// This next function will be called during decoding of the jpeg file to render each
// 16x16 or 8x8 image tile (Minimum Coding Unit) to the TFT.
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
   // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // STM32F767 processor takes 43ms just to decode (and not draw) jpeg (-Os compile option)
  // Total time to decode and also draw to TFT:
  // SPI 54MHz=71ms, with DMA 50ms, 71-43 = 28ms spent drawing, so DMA is complete before next MCU block is ready
  // Apparent performance benefit of DMA = 71/50 = 42%, 50 - 43 = 7ms lost elsewhere
  // SPI 27MHz=95ms, with DMA 52ms. 95-43 = 52ms spent drawing, so DMA is *just* complete before next MCU block is ready!
  // Apparent performance benefit of DMA = 95/52 = 83%, 52 - 43 = 9ms lost elsewhere
#ifdef USE_DMA
  // Double buffering is used, the bitmap is copied to the buffer by pushImageDMA() the
  // bitmap can then be updated by the jpeg decoder while DMA is in progress
  if (dmaBufferSel) dmaBufferPtr = dmaBuffer2;
  else dmaBufferPtr = dmaBuffer1;
  dmaBufferSel = !dmaBufferSel; // Toggle buffer selection
  //  pushImageDMA() will clip the image block at screen boundaries before initiating DMA
  tft.pushImageDMA(x, y, w, h, bitmap, dmaBufferPtr); // Initiate DMA - blocking only if last DMA is not complete
  // The DMA transfer of image block to the TFT is now in progress...
#else
  // Non-DMA blocking alternative
  tft.pushImage(x, y, w, h, bitmap);  // Blocking, so only returns when image block is drawn
#endif
  // Return 1 to decode next block.
  return 1;
}



// Comment out to stop drawing black spots
#define BLACK_SPOT

// Switch position and size
#define FRAME_X 100
#define FRAME_Y 64
#define FRAME_W 120
#define FRAME_H 50

// Red zone size
#define REDBUTTON_X FRAME_X
#define REDBUTTON_Y FRAME_Y
#define REDBUTTON_W (FRAME_W/2)
#define REDBUTTON_H FRAME_H

// Green zone size
#define GREENBUTTON_X (REDBUTTON_X + REDBUTTON_W)
#define GREENBUTTON_Y FRAME_Y
#define GREENBUTTON_W (FRAME_W/2)
#define GREENBUTTON_H FRAME_H

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void setup(void)
{
  Serial.begin(115200);
  tft.init();  // Set the rotation before we calibrate
  tft.setRotation(1);  // call screen calibration
  touch_calibrate();

  // clear screen
  tft.fillScreen(0x0000);

  #ifdef USE_DMA
  tft.initDMA(); // To use SPI DMA you must call initDMA() to setup the DMA engine
  #endif

  // The jpeg image can be scaled down by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(1);

  // The colour byte order can be swapped by the decoder
  // using TJpgDec.setSwapBytes(true); or by the TFT_eSPI library:
  tft.setSwapBytes(true);

  // The decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);
  
    pinMode(32, OUTPUT); 
    
    pinMode(releRes, OUTPUT);
    pinMode(releAgua, OUTPUT);
 
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
      if (! rtc.begin()) {
      Serial.println("Couldn't find RTC");
      Serial.flush();
      abort();
      }
    
    if (rtc.lostPower()) {
        Serial.println("RTC lost power, let's set the time!");
        // When time needs to be set on a new device, or after a power loss, the
        // following line sets the RTC to the date & time this sketch was compiled
       rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
       // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
     }
    
  // Draw button (this example does not use library Button class)
  mainMenu();
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
int value = 0;
int statusMenu = 1;

void loop()
{
  uint16_t x, y, z;
  WiFiClient client = server.available();   // listen for incoming clients
  DateTime now = rtc.now();
  minuteNow = now.minute();
              
  tft.getTouchRaw(&x, &y);
  z = tft.getTouchRawZ();
  Serial.printf("x: %i     ", x);
  Serial.printf("y: %i     ", y);
  Serial.printf("z: %i     ", z);

  
  Serial.println();
  delay(10);

//  Serial.println(statusMenu);

    if((minuteNow!=minutePrevious) || (z > 300 ))
      {
       if (statusMenu == 1){
          mainMenu();
           if ((x > 2320) && (x < 3450))
                    
                { 
                  if ((y > 840) && (y <1700)) 
                  {  
                    Serial.println("Fazer Café");
                    mainMenu2();
                  }
                 }else if ((x > 520) && (x < 1620))
                             
                           {
                            if ((y > 2730) && (y <3550) && (z > 400)) 
                            {  
                                Serial.println("Agendamentos");
                                mainMenu5();
                            }  
                            if ((y > 820) && (y <1710)) 
                                     {  
                                     Serial.println("programar");

                                      DateTime now = rtc.now();
                                      minuteNow2 = now.minute();
                                      horaNow = now.hour();
          
                                      mainMenu4(minuteNow2, horaNow);
                                      statusMenu = 4; 
                                         }
                                      }
                 
        }else if(statusMenu == 2){
          mainMenu2();
                  if ((x > 3400) && (x < 3675))
                  { 
                  if ((y > 440) && (y <680)) 
                  {  
                    Serial.println("Home");
                    mainMenu();
                  }
          

                  }else if ((x > 2100) && (x <3550)&& (z > 400)) //quantidade
                                                                { if ((y > 2500) && (y <3800)) {  
                                                                                                      quantidade = 200;
                                                                                                      mainMenu3();
                                                                                               }
                          
                                                                 if ((y > 850) && (y <2200)) {  
                                                                                                    quantidade = 400;
                                                                                                    mainMenu3();
                                                                }
                                                                
                                                                }else if ((x > 0) && (x <1750)&& (z > 400)) //verifica quantidade
                                                                                                            { if ((y > 2470) && (y <3800)) 
                                                                                                                                          {  
                                                                                                                                           quantidade = 600;
                                                                                                                                           mainMenu3();
                                                                                                             } if ((y > 850) && (y <2180)) //quantidade
                                                                                                                                   {
                                                                                                                                    quantidade = 800;
                                                                                                                                    mainMenu3();
                                                                                                                                    }
                                                               }
              Serial.println(quantidade);            
                  
       }else if(statusMenu == 3){
          mainMenu3();
          
       }else if(statusMenu == 4){
                  
                  if ((x > 2050) && (x <2500)&& (z > 400)) //verifica o hora ou minuto +
                  {  
                      if ((y > 3030) && (y <3330)) //verifica o hora +
                      {  
                        if (horaNow < 23){
                          horaNow = horaNow+1;
                        }else   {
                              horaNow = 00;
                          }
                          
                          }else if ((y > 2000) && (y <2320)) //verifica o minuto +
                          {  
                             if (minuteNow2 < 59){
                              minuteNow2 = minuteNow2+1;
                             }else
                              {
                                minuteNow2 = 00;
                            }
                          
                          }
                      }else if ((x > 0) && (x <690)&& (z > 400)) //verifica o hora ou minuto -
                  {  
                      if ((y > 3030) && (y <3330)) //verifica o hora -
                      {  
                        if (horaNow > 0){
                          horaNow = horaNow-1;
                        }else   {
                              horaNow = 23;
                          }
                          
                          }else if ((y > 2000) && (y <2320)) //verifica o minuto -
                          {  
                             if (minuteNow2 > 0){
                              minuteNow2 = minuteNow2-1;
                             }else
                              {
                                minuteNow2 = 59;
                            }
                          
                          }
                      }else if ((x > 3400) && (x < 3675)&& (z > 400))
                  { 
                    if ((y > 440) && (y <680)) 
                    {  
                     Serial.println("Home 1");
                     statusMenu = 1;
                     mainMenu();
//                     Serial.println(statusMenu);
                    }
                  }

                  mainMenu4(minuteNow2, horaNow);



                  
       }else if(statusMenu == 5)
       {
         mainMenu5();
                           if ((x > 3400) && (x < 3675))
                  { 
                  if ((y > 440) && (y <680)) 
                  {  
                    Serial.println("Home");
                    statusMenu = 1;
                    mainMenu();
                  }
                  }
       }
       }

  
 
            
           
          
          


   
//    Serial.print(now.year(), DEC);
//    Serial.print('/');
//    Serial.print(now.month(), DEC);
//    Serial.print('/');
//    Serial.print(now.day(), DEC);
//    Serial.print(" (");
//    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
//    Serial.print(") ");
//    Serial.print(now.hour(), DEC);
//    Serial.print(':');
//    Serial.print(now.minute(), DEC);
//    hs = now.hour();
//    mi = now.minute();
//
//    Serial.println(hs);
//    Serial.println(mi);
//    
//
//    Serial.println();
  

    //if (statusMenu == 1){
      
    //}
    
    if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
      
      int luminosidade = 100;

       delay(4000);

      
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


    Serial.println(F("Disconnecting from client"));
  
    }
    
  
  // See if there's any touch data for us



//

  
//   if (tft.getTouch(&x, &y))
//  {
    // Draw a block spot to show where touch was calculated to be
//    #ifdef BLACK_SPOT
//      tft.fillCircle(x, y, 2, TFT_BLACK);
//    #endif
    


  

//    else //Record is off (SwitchOn == false)
//    {
//      if ((x > GREENBUTTON_X) && (x < (GREENBUTTON_X + GREENBUTTON_W))) {
//        if ((y > GREENBUTTON_Y) && (y <= (GREENBUTTON_Y + GREENBUTTON_H))) {
//          Serial.println("Green btn hit");
//          greenBtn();
//        }
//      }
//    }
//
//    Serial.println(SwitchOn);
//
//  }
}
//------------------------------------------------------------------------------------------

void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists
  if (!SPIFFS.begin()) {
    Serial.println("Formating file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft.setTouch(calData);
  } else {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}

void drawFrame()
{
  tft.drawRect(FRAME_X, FRAME_Y, FRAME_W, FRAME_H, TFT_BLACK);
}


// Draw a red button
void mainMenu()
{
  statusMenu = 1;
  uint16_t w = 0, h = 0;
  uint16_t x, y;
              
              DateTime now = rtc.now();
              minuteNow = now.minute();
              if(minuteNow!=minutePrevious)
            {
//              dateString = daysOfTheWeek[now.dayOfTheWeek()]+", ";
//              dateString = dateString+String(now.day())+"/"+String(now.month());
//              dateString= dateString+"/"+ String(now.year()); 
              minutePrevious = minuteNow;
              String hours= "";
              if(now.hour()<10)
              {
                 hours = hours+"0"+String(now.hour());
                }else
              {
                hours = hours+""+String(now.hour());
              }
              if(now.minute()<10)
              {
                hours = hours+":0"+String(now.minute());
              }else
              {
                hours = hours+":"+String(now.minute());
              }
              

              timeChar = hours;
//              hours.toCharArray(timeChar);
              Serial.println(timeChar);

         //     tft.fillRect(10,0,160,65,ST7735_BLACK);
         //     printText(timeChar, ST7735_WHITE,20,25,3);
         //     dateString.toCharArray(dateChar,50);
         //     printText(dateChar, ST7735_GREEN,8,5,1);

            }
  
  TJpgDec.getJpgSize(&w, &h, menu1, sizeof(menu1));

  // Must use startWrite first so TFT chip select stays low during DMA and SPI channel settings remain configured
  tft.startWrite();

  // Draw the image, top left at 0,0 - DMA request is handled in the call-back tft_output() in this sketch
  TJpgDec.drawJpg(0, 0, menu1, sizeof(menu1));

  // Must use endWrite to release the TFT chip select and release the SPI channel
  tft.endWrite();

 tft.setTextColor(TFT_BLACK);
 tft.setTextSize(3);
 tft.drawString(timeChar,30,55); 
}


void mainMenu2()
{

  statusMenu = 2;
  uint16_t w = 0, h = 0;
  TJpgDec.getJpgSize(&w, &h, menu2, sizeof(menu2));

  // Must use startWrite first so TFT chip select stays low during DMA and SPI channel settings remain configured
  tft.startWrite();

  // Draw the image, top left at 0,0 - DMA request is handled in the call-back tft_output() in this sketch
  TJpgDec.drawJpg(0, 0, menu2, sizeof(menu2));

  // Must use endWrite to release the TFT chip select and release the SPI channel
    tft.endWrite();
}


void  mainMenu3()
{

  uint16_t w = 0, h = 0;
  statusMenu = 3;
  TJpgDec.getJpgSize(&w, &h, menu3, sizeof(menu3));

  // Must use startWrite first so TFT chip select stays low during DMA and SPI channel settings remain configured
  tft.startWrite();

  // Draw the image, top left at 0,0 - DMA request is handled in the call-back tft_output() in this sketch
  TJpgDec.drawJpg(0, 0, menu3, sizeof(menu3));

  // Must use endWrite to release the TFT chip select and release the SPI channel
  tft.endWrite();

}

void  mainMenu4(int minn, int horan)
{

  uint16_t w = 0, h = 0, z= 0;
//  statusMenu = 4;
  
              String agenda= "";
              if(horan<10)
              {
                 agenda = agenda+"0"+String(horan);
                }else
              {
                agenda = agenda+""+String(horan);
              }
              if(minn<10)
              {
                agenda = agenda+":0"+String(minn);
              }else
              {
                agenda = agenda+":"+String(minn);
              }
              timeChar2 = agenda;

              
  TJpgDec.getJpgSize(&w, &h, menu4, sizeof(menu4));

  // Must use startWrite first so TFT chip select stays low during DMA and SPI channel settings remain configured
  tft.startWrite();

  // Draw the image, top left at 0,0 - DMA request is handled in the call-back tft_output() in this sketch
  TJpgDec.drawJpg(0, 0, menu4, sizeof(menu4));

  // Must use endWrite to release the TFT chip select and release the SPI channel
  tft.endWrite();

   tft.setTextColor(TFT_BLACK);
   tft.setTextSize(4);
   tft.drawString(timeChar2,60,150);

   


}


void  mainMenu5()
{

  uint16_t w = 0, h = 0;
  statusMenu = 5;
  TJpgDec.getJpgSize(&w, &h, menu5, sizeof(menu5));

  // Must use startWrite first so TFT chip select stays low during DMA and SPI channel settings remain configured
  tft.startWrite();

  // Draw the image, top left at 0,0 - DMA request is handled in the call-back tft_output() in this sketch
  TJpgDec.drawJpg(0, 0, menu5, sizeof(menu5));

  // Must use endWrite to release the TFT chip select and release the SPI channel
  tft.endWrite();

}

void  preparaCafe()
{

  digitalWrite(releRes,0); //Liga Resistencia
  delay(1000); //aguarda x Seg
  digitalWrite(releAgua,0); //Liga Aguá

  if(temperatura > 80){
    
    
    }
  
  
  
  
}
}

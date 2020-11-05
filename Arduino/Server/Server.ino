#include "Arduino.h"
#include <FS.h>
#include "Free_Fonts.h" 
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
#include "menu6.h"
#include <locale.h>




#include <TJpg_Decoder.h>

#define USE_DMA

  
#ifdef USE_DMA
  uint16_t  dmaBuffer1[16*16]; // Toggle buffer for 16*16 MCU block, 512bytes
  uint16_t  dmaBuffer2[16*16]; // Toggle buffer for 16*16 MCU block, 512bytes
  uint16_t* dmaBufferPtr = dmaBuffer1;
  bool dmaBufferSel = 0;
#endif



RTC_DS3231 rtc;
char diasDaSemana[7][12] = {"Dom", "Seg", "Ter", "Quar", "Quin", "Sex", "Sáb"};
String timeChar, timeChar2, diaN, diaS;
char dateChar[50];
char temperatureChar[10];
String dateString;
int minuteNow=0, minuteNow2=0, horaNow=0, horaNow2=0, diaNow=0;
int minutePrevious=0;
int tempo;
int preparoApp=0;

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

const int q800 = 740;
const int q600 = 555;
const int q400 = 370;
const int q200 = 185;


//boolean SwitchOn = false;

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
//
//// Switch position and size
//#define FRAME_X 100
//#define FRAME_Y 64
//#define FRAME_W 120
//#define FRAME_H 50
//
//// Red zone size
//#define REDBUTTON_X FRAME_X
//#define REDBUTTON_Y FRAME_Y
//#define REDBUTTON_W (FRAME_W/2)
//#define REDBUTTON_H FRAME_H
//
//// Green zone size
//#define GREENBUTTON_X (REDBUTTON_X + REDBUTTON_W)
//#define GREENBUTTON_Y FRAME_Y
//#define GREENBUTTON_W (FRAME_W/2)
//#define GREENBUTTON_H FRAME_H
#define MAX 3

struct Agendamentos{
            uint8_t  diasAgenda;
            uint8_t  horaAgenda;
            uint8_t  minutoAgenda;
            int  quantidadeAgenda;
            int tempoAgenda;
            
};

struct Agendamentos a1, a2, a3;

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void setup(void)
{
  Serial.begin(115200);
  tft.init();  // Set the rotation before we calibrate
  tft.setRotation(1);  // call screen calibration
  touch_calibrate();

  setlocale(LC_ALL, "Portuguese");

  a1.diasAgenda = 0;
  a2.diasAgenda = 0;
  a3.diasAgenda = 0;
  
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
uint8_t  diasTemp;
uint8_t  horaTemp;
uint8_t  minutoTemp;
int  quantidadeTemp;
int tempoTemp;

void loop()
{
  uint16_t x, y, z;
  WiFiClient client = server.available();   // listen for incoming clients
  DateTime now = rtc.now();
  minuteNow = now.minute();
  horaNow = now.hour();
  diaNow = now.dayOfTheWeek();

  
              
  tft.getTouchRaw(&x, &y);
  z = tft.getTouchRawZ();
//  Serial.printf("x: %i     ", x);
//  Serial.printf("y: %i     ", y);
//  Serial.printf("z: %i     ", z);

  
//  Serial.println(statusMenu);
  delay(10);

//  Serial.println(statusMenu);

  if (verificaAlarme(a1, a2, a3, minuteNow, horaNow, diaNow) == 1) {
    Serial.println("Run Bitch, tem café pra passar!");
    
    preparaCafe(a1.tempoAgenda);
  } else if (verificaAlarme(a1, a2, a3, minuteNow, horaNow, diaNow) == 2) {
    Serial.println("Run Bitch, tem café pra passar!");
    preparaCafe(a2.tempoAgenda);
  } else if (verificaAlarme(a1, a2, a3, minuteNow, horaNow, diaNow) == 3) {
    Serial.println("Run Bitch, tem café pra passar!");
    preparaCafe(a3.tempoAgenda);
  }

// Verifica preparo App
  if (preparoApp!= 0){
      preparaCafe(preparoApp);
    }


     if ((minuteNow != minutePrevious) || (z > 300)) {
      if (statusMenu == 1) {
        mainMenu();
        if ((x > 2320) && (x < 3450))

        {
          if ((y > 840) && (y < 1700)) {
            Serial.println("Fazer Café");
            statusMenu = 2;
            mainMenu2();
          }
        } else if ((x > 520) && (x < 1620))

        {
          if ((y > 2730) && (y < 3550) && (z > 400)) {
            Serial.println("Agendamentos");
            mainMenu5(a1, a2, a3);
          }
          if ((y > 820) && (y < 1710)) {
            Serial.println("programar");

            DateTime now = rtc.now();
            minuteNow2 = now.minute();
            horaNow2 = now.hour();

            statusMenu = 4;
            mainMenu4();

          }
        }

      } else if (statusMenu == 2) {
//        mainMenu2();
        if ((x > 3400) && (x < 3675)) {
          if ((y > 440) && (y < 680)) {
            Serial.println("Home");
            statusMenu = 1;
            mainMenu();
          }

        } else if ((x > 2100) && (x < 3550) && (z > 400)) //quantidade
        {
          if ((y > 2500) && (y < 3800)) {
            quantidade = q200;
            tempo = 185;
            preparaCafe(tempo);

          }

          if ((y > 850) && (y < 2200)) {
            quantidade = q400;
            tempo = 370;
            preparaCafe(tempo);
            //mainMenu3();
          }

        } else if ((x > 0) && (x < 1750) && (z > 400)) //verifica quantidade
        {
          if ((y > 2470) && (y < 3800)) {
            quantidade = q600;
            tempo = 555;
            preparaCafe(tempo);
            //mainMenu3();
          }
          if ((y > 850) && (y < 2180)) //quantidade
          {
            quantidade = q800;
            tempo = 740;
            preparaCafe(tempo);
            //mainMenu3();
          }
        }
        Serial.println(quantidade);

      } else if (statusMenu == 3) {
        //  mainMenu3();

      } else if (statusMenu == 4) {
//        mainMenu4();
         if ((x > 3400) && (x < 3675)) {
          if ((y > 440) && (y < 680)) {
            Serial.println("Home");
            statusMenu = 1;
            mainMenu();
          }
         }
      else if ((x > 2500) && (x < 3100) && (z > 400)) //quantidade
        {
          if ((y > 750) && (y < 3670)) {
            diasTemp = 5;
            Serial.println("Dias da Semana");
            statusMenu = 6;
            mainMenu6(minuteNow2, horaNow2);
          }

        } else if ((x > 1620) && (x < 2000) && (z > 400)) //quantidade
        {
          if ((y > 2640) && (y < 3670)) {
            diasTemp = 6;
            Serial.println("Sábado");
            statusMenu = 6;
            mainMenu6(minuteNow2, horaNow2);
          } else if ((y > 690) && (y < 1750)) {
            diasTemp = 7;
            Serial.println("Domingo");
            statusMenu = 6;
            mainMenu6(minuteNow2, horaNow2);
          }
        } else if ((x > 500) && (x < 1050) && (z > 400)) //quantidade
        {
          if ((y > 690) && (y < 3620)) {
            diasTemp = 8;
            Serial.println("Finais de Semana");
            statusMenu = 6;
            mainMenu6(minuteNow2, horaNow2);
          }

        }
      } else if (statusMenu == 6) {
          mainMenu6(minuteNow2, horaNow2);
        if ((x > 2050) && (x < 2500) && (z > 400)) //verifica o hora ou minuto +
        {
          if ((y > 3030) && (y < 3330)) //verifica o hora +
          {
            if (horaNow2 < 23) {
              horaNow2 = horaNow2 + 1;
            } else {
              horaNow2 = 00;
            }

          } else if ((y > 2000) && (y < 2320)) //verifica o minuto +
          {
            if (minuteNow2 < 59) {
              minuteNow2 = minuteNow2 + 1;
            } else {
              minuteNow2 = 00;
            }

          }
        } else if ((x > 0) && (x < 690) && (z > 400)) //verifica o hora ou minuto -
        {
          if ((y > 3030) && (y < 3330)) //verifica o hora -
          {
            if (horaNow2 > 0) {
              horaNow2 = horaNow2 - 1;
            } else {
              horaNow2 = 23;
            }

          } else if ((y > 2000) && (y < 2320)) //verifica o minuto -
          {
            if (minuteNow2 > 0) {
              minuteNow2 = minuteNow2 - 1;
            } else {
              minuteNow2 = 59;
            }

          }
        } else if ((x > 3400) && (x < 3675) && (z > 400)) {
          if ((y > 440) && (y < 680)) {
            Serial.println("Home 1");
            statusMenu = 1;
            mainMenu();
            //                     Serial.println(statusMenu);
          }

        } else if ((x > 2620) && (x < 3100) && (z > 400)) {
          if ((y > 750) && (y < 1616)) {
            Serial.println("OK");
            minutoTemp = minuteNow2;
            horaTemp = horaNow2;
            statusMenu = 7;
            mainMenu2();
            //                     Serial.println(statusMenu);
          }  
        }
        
      }else if (statusMenu == 5) {
      
        if (z > 300){
          mainMenu5(a1, a2, a3);
        }if ((x > 3400) && (x < 3675)) {
            if ((y > 440) && (y < 680)) {
              Serial.println("Home");
              statusMenu = 1;
              mainMenu();
            }
          }else if((y > 490) && (y < 800) && (z > 400)) //verifica quantidade
          {
            if ((x > 2410) && (x < 3100)) {
              
              a1.diasAgenda = 0;
              tft.fillScreen(TFT_WHITE);
              tft.setTextColor(TFT_BLACK);
              tft.setTextSize(2);
              tft.drawString("Agendamento excluído",40,120);
              delay(3000);
              
              mainMenu();
            }else if ((x > 1480) && (x < 2150)) {
              
              a2.diasAgenda = 0;
              tft.fillScreen(TFT_WHITE);
              tft.setTextColor(TFT_BLACK);
              tft.setTextSize(2);
              tft.drawString("Agendamento excluído",40,120);
              delay(3000);
              
              mainMenu();
            }else if ((x > 500) && (x < 1170)) {
              
              a3.diasAgenda = 0;
              tft.fillScreen(TFT_WHITE);
              tft.setTextColor(TFT_BLACK);
              tft.setTextSize(2);
              tft.drawString("Agendamento excluído",40,120);
              delay(3000);
              
              mainMenu();
            }
          }

        } else if (statusMenu == 7) {
          mainMenu2();
          if ((x > 3400) && (x < 3675)) {
            if ((y > 440) && (y < 680)) {
              Serial.println("Home");
              statusMenu = 1;
              mainMenu();
            }

          } else if ((x > 2100) && (x < 3550) && (z > 400)) //quantidade
          {
            if ((y > 2500) && (y < 3800)) {
              quantidadeTemp = 200;
              tempoTemp = q200;
              criaAgendamento(  diasTemp,  horaTemp,  minutoTemp,  quantidadeTemp  , tempoTemp, &a1, &a2, &a3);
              mainMenu();

            }

            if ((y > 850) && (y < 2200)) {
              quantidadeTemp = 400;
              tempoTemp = q400;
              criaAgendamento(  diasTemp,  horaTemp,  minutoTemp,  quantidadeTemp  , tempoTemp, &a1, &a2, &a3);
              mainMenu();

              //mainMenu3();
            }

          } else if ((x > 0) && (x < 1750) && (z > 400)) //verifica quantidade
          {
            if ((y > 2470) && (y < 3800)) {
              quantidadeTemp = 600;
              tempoTemp = q600;
              criaAgendamento(  diasTemp,  horaTemp,  minutoTemp,  quantidadeTemp  , tempoTemp, &a1, &a2, &a3);
              mainMenu();

              //mainMenu3();
            }
            if ((y > 850) && (y < 2180)) //quantidade
            {
              quantidadeTemp = 800;
              tempoTemp = q800;
              criaAgendamento(  diasTemp,  horaTemp,  minutoTemp,  quantidadeTemp  , tempoTemp, &a1, &a2, &a3);
              mainMenu();

              //mainMenu3();
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
        
//      int luminosidade = 100;

//       delay(4000);

        String retorno;
        String request = client.readStringUntil('\r');
        Serial.println(request);
//        client.flush();

         // analisar os dados que chegaram
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println();
          
        if (request.indexOf(F("make_800")) != -1) {
          Serial.println("Preapara 800 ml de café");  
          preparoApp = q800;
          client.println("OK,800");
          // delay(200);           
        } 

        if (request.indexOf(F("make_600")) != -1) {
          Serial.println("Preapara 600 ml de café");  
          preparoApp = q600;
          client.println("OK,600");
          // delay(200);           
        } 

        if (request.indexOf(F("make_400")) != -1) {
          Serial.println("Preapara 400 ml de café");  
          preparoApp = q400;
          client.println("OK,400");
          // delay(200);           
        } 

        if (request.indexOf(F("make_200")) != -1) {
          Serial.println("Preapara 200 ml de café");  
          preparoApp = q200;
          client.println("OK,200");
          // delay(200);           
        } 



        if (request.indexOf(F("get_agendamentos")) != -1) {
          Serial.println("pegando Agendamentos");  
         // preparoApp = q200;
          retorno = retornaAgendamentoApp(a1, a2, a3);
          Serial.println(retorno);
          client.println(retorno);
          // delay(200);           
        }

       if (request.indexOf(F("del_age1")) != -1) {  
          a1.diasAgenda = 0;
          retorno = retornaAgendamentoApp(a1, a2, a3);
          Serial.println(retorno);
          client.println(retorno);
          // delay(200);           
        }

          if (request.indexOf(F("del_age2")) != -1) {  
          a2.diasAgenda = 0;
          retorno = retornaAgendamentoApp(a1, a2, a3);
          Serial.println(retorno);
          client.println(retorno);
          // delay(200);           
        }

          if (request.indexOf(F("del_age3")) != -1) {  
          a3.diasAgenda = 0;
          retorno = retornaAgendamentoApp(a1, a2, a3);
          Serial.println(retorno);
          client.println(retorno);
          // delay(200);           
        }


          if (request.indexOf(F("age_")) != -1) {  
          String diaR, horaR, minutoR, quantidadeR;
          int diaRa = 0, horaRa = 0, minutoRA = 0, quantidadeRA=0, tempoRA=0;
          
          char buffer[26];
          request.toCharArray(buffer, 26);
          
          diaR = strtok(buffer, "/");
          diaR = strtok(NULL, ":");
          horaR = strtok(NULL, ":");
          minutoR = strtok(NULL, ":");
          quantidadeR = strtok(NULL, ":");


          if (diaR == "age_diasda"){
            diaRa = 5;
            }         else if (diaR == "age_sabado"){
              diaRa = 6;
            }         else if (diaR == "age_doming"){
              diaRa = 7;
            }         else if (diaR == "age_finais"){
              diaRa = 8;
            }

            if (quantidadeR == "200"){
            quantidadeTemp = 200;
            tempoTemp = q200;
            }         else if (quantidadeR == "400"){
            quantidadeTemp = 400;
            tempoTemp = q400;
            }         else if (quantidadeR == "600"){
                        quantidadeTemp = 600;
            tempoTemp = q600;
            }         else if (quantidadeR == "800"){
                       quantidadeTemp = 800;
            tempoTemp = q800;
            }

            horaRa = horaR.toInt();
            minutoRA = minutoR.toInt();


          
          if (criaAgendamento(diaRa,  horaRa,  minutoRA,  quantidadeTemp  , tempoTemp, &a1, &a2, &a3) == 1 ){
            client.println("Cheio");
            }
          
          Serial.println(quantidadeR);
           
        }

       // Serial.println(request);
        
//          client.println("Oi");


  //  Serial.println(F("Disconnecting from client"));
  
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

//void drawFrame()
//{
//  tft.drawRect(FRAME_X, FRAME_Y, FRAME_W, FRAME_H, TFT_BLACK);
//}


// Draw a red button
void mainMenu()
{
  statusMenu = 1;
  uint16_t w = 0, h = 0;
  uint16_t x, y;
              
              DateTime now = rtc.now();
              minuteNow = now.minute();
              
              String diaS= "";
              String diaN= "";

              diaN = diaN+String(now.day());
            //  Serial.println(diaN);
            //  Serial.println(now.day());
              
              diaS = diaS+String(diasDaSemana[now.dayOfTheWeek()]);
              
              Serial.println(diaS);
              Serial.println(diasDaSemana[now.dayOfTheWeek()]);
              
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
//            hours.toCharArray(timeChar);
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


  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.drawString(diaS,57,140); 


  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(3);
  tft.drawString(diaN,55,180);
  

}


void mainMenu2()
{

//  statusMenu = 2;
  uint16_t w = 0, h = 0;
  TJpgDec.getJpgSize(&w, &h, menu2, sizeof(menu2));

  // Must use startWrite first so TFT chip select stays low during DMA and SPI channel settings remain configured
  tft.startWrite();

  // Draw the image, top left at 0,0 - DMA request is handled in the call-back tft_output() in this sketch
  TJpgDec.drawJpg(0, 0, menu2, sizeof(menu2));

  // Must use endWrite to release the TFT chip select and release the SPI channel
    tft.endWrite();
}


void  mainMenu3(int tmp, float tmpC)
{
  //String timeRes =  String(tmp); 
  String timeRes = "";
  uint16_t w = 0, h = 0;
  int ho, m, s;
  statusMenu = 3;


  String tempeC =  String(tmpC)+(char)247+"C";
  ho = (tmp/3600); 
  m = (tmp -(3600*ho))/60;
  s = (tmp -(3600*ho)-(m*60));

  if(m<10)
              {
                 timeRes = "0"+String(m);
              }else
              {
                  timeRes = String(m);
              }
       if(s<10)
              {
                 timeRes = timeRes+":0"+String(s);
              }else
              {
                timeRes =  timeRes+":"+String(s);
              }
  
  
  TJpgDec.getJpgSize(&w, &h, menu3, sizeof(menu3));

  // Must use startWrite first so TFT chip select stays low during DMA and SPI channel settings remain configured
  tft.startWrite();

  // Draw the image, top left at 0,0 - DMA request is handled in the call-back tft_output() in this sketch
  TJpgDec.drawJpg(0, 0, menu3, sizeof(menu3));

  // Must use endWrite to release the TFT chip select and release the SPI channel
  tft.endWrite();
     
   
   tft.setTextColor(TFT_BLACK);
   tft.setTextSize(5);
   tft.drawString(timeRes,35,115);
   tft.setTextColor(TFT_WHITE);
   tft.setTextSize(1);
   tft.drawString(tempeC,115,226);
   

}

//void  mainMenu4(struct Agendamentos *a1s, struct Agendamentos *a2s, struct Agendamentos *a3s)
void  mainMenu4()
{

  uint16_t w = 0, h = 0, z= 0;
//  statusMenu = 4;
              
  TJpgDec.getJpgSize(&w, &h, menu4, sizeof(menu4));

  // Must use startWrite first so TFT chip select stays low during DMA and SPI channel settings remain configured
  tft.startWrite();

  // Draw the image, top left at 0,0 - DMA request is handled in the call-back tft_output() in this sketch
  TJpgDec.drawJpg(0, 0, menu4, sizeof(menu4));

  // Must use endWrite to release the TFT chip select and release the SPI channel
  tft.endWrite();
  
            


}

void  mainMenu5(struct Agendamentos a1s, struct Agendamentos a2s, struct Agendamentos a3s)
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
  
  if (a1s.diasAgenda == 0 ){
          tft.setTextColor(TFT_BLACK);
          tft.setTextSize(2);
          tft.drawString("Vazio",20,60);
    } else if (a1s.diasAgenda == 5) {
              String hoursd= "";
              if(a1s.horaAgenda<10)
              {
                 hoursd = hoursd+"0"+String(a1s.horaAgenda);
                }else
              {
                hoursd = hoursd+""+String(a1s.horaAgenda);
              }
              if(a1s.minutoAgenda<10)
              {
                hoursd = hoursd+":0"+String(a1s.minutoAgenda);
              }else
              {
                hoursd = hoursd+":"+String(a1s.minutoAgenda);
              }
          tft.setTextColor(TFT_BLACK);
          tft.setTextSize(2);
          tft.drawString("Dias da Semana",20,60);
          tft.drawString(hoursd,213,60);
      
      }else if (a1s.diasAgenda == 6) {
              String hoursd= "";
              if(a1s.horaAgenda<10)
              {
                 hoursd = hoursd+"0"+String(a1s.horaAgenda);
                }else
              {
                hoursd = hoursd+""+String(a1s.horaAgenda);
              }
              if(a1s.minutoAgenda<10)
              {
                hoursd = hoursd+":0"+String(a1s.minutoAgenda);
              }else
              {
                hoursd = hoursd+":"+String(a1s.minutoAgenda);
              }
          tft.setTextColor(TFT_BLACK);
          tft.setTextSize(2);
          tft.drawString("Sábados",20,60);
          tft.drawString(hoursd,213,60);
      
      }else if (a1s.diasAgenda == 7) {
              String hoursd= "";
              if(a1s.horaAgenda<10)
              {
                 hoursd = hoursd+"0"+String(a1s.horaAgenda);
                }else
              {
                hoursd = hoursd+""+String(a1s.horaAgenda);
              }
              if(a1s.minutoAgenda<10)
              {
                hoursd = hoursd+":0"+String(a1s.minutoAgenda);
              }else
              {
                hoursd = hoursd+":"+String(a1s.minutoAgenda);
              }
          tft.setTextColor(TFT_BLACK);
          tft.setTextSize(2);
          tft.drawString("Domingos",20,60);
          tft.drawString(hoursd,213,60);
      
      }else if (a1s.diasAgenda == 8) {
              String hoursd= "";
              if(a1s.horaAgenda<10)
              {
                 hoursd = hoursd+"0"+String(a1s.horaAgenda);
                }else
              {
                hoursd = hoursd+""+String(a1s.horaAgenda);
              }
              if(a1s.minutoAgenda<10)
              {
                hoursd = hoursd+":0"+String(a1s.minutoAgenda);
              }else
              {
                hoursd = hoursd+":"+String(a1s.minutoAgenda);
              }
          tft.setTextColor(TFT_BLACK);
          tft.setTextSize(2);
          tft.drawString("Finais de semana",10,60);
          tft.drawString(hoursd,213,60);
      
      }

    if (a2s.diasAgenda == 0 ){
          tft.setTextColor(TFT_BLACK);
          tft.setTextSize(2);
          tft.drawString("Vazio",20,130);
    } else if (a2s.diasAgenda == 5) {
              String hoursd= "";
              if(a2s.horaAgenda<10)
              {
                 hoursd = hoursd+"0"+String(a2s.horaAgenda);
                }else
              {
                hoursd = hoursd+""+String(a2s.horaAgenda);
              }
              if(a2s.minutoAgenda<10)
              {
                hoursd = hoursd+":0"+String(a2s.minutoAgenda);
              }else
              {
                hoursd = hoursd+":"+String(a2s.minutoAgenda);
              }
          tft.setTextColor(TFT_BLACK);
          tft.setTextSize(2);
          tft.drawString("Dias da Semana",20,130);
          tft.drawString(hoursd,213,130);
      
      }else if (a2s.diasAgenda == 6) {
              String hoursd= "";
              if(a2s.horaAgenda<10)
              {
                 hoursd = hoursd+"0"+String(a2s.horaAgenda);
                }else
              {
                hoursd = hoursd+""+String(a2s.horaAgenda);
              }
              if(a2s.minutoAgenda<10)
              {
                hoursd = hoursd+":0"+String(a2s.minutoAgenda);
              }else
              {
                hoursd = hoursd+":"+String(a2s.minutoAgenda);
              }
          tft.setTextColor(TFT_BLACK);
          tft.setTextSize(2);
          tft.drawString("Sábados",20,130);
          tft.drawString(hoursd,213,130);
      
      }else if (a2s.diasAgenda == 7) {
              String hoursd= "";
              if(a2s.horaAgenda<10)
              {
                 hoursd = hoursd+"0"+String(a2s.horaAgenda);
                }else
              {
                hoursd = hoursd+""+String(a2s.horaAgenda);
              }
              if(a2s.minutoAgenda<10)
              {
                hoursd = hoursd+":0"+String(a2s.minutoAgenda);
              }else
              {
                hoursd = hoursd+":"+String(a2s.minutoAgenda);
              }
          tft.setTextColor(TFT_BLACK);
          tft.setTextSize(2);
          tft.drawString("Domingos",20,130);
          tft.drawString(hoursd,213,130);
      
      }else if (a2s.diasAgenda == 8) {
              String hoursd= "";
              if(a2s.horaAgenda<10)
              {
                 hoursd = hoursd+"0"+String(a2s.horaAgenda);
                }else
              {
                hoursd = hoursd+""+String(a2s.horaAgenda);
              }
              if(a2s.minutoAgenda<10)
              {
                hoursd = hoursd+":0"+String(a2s.minutoAgenda);
              }else
              {
                hoursd = hoursd+":"+String(a2s.minutoAgenda);
              }
          tft.setTextColor(TFT_BLACK);
          tft.setTextSize(2);
          tft.drawString("Finais de semana",10,130);
          tft.drawString(hoursd,213,130);
      
      }
    
    if (a3s.diasAgenda == 0 ){
          tft.setTextColor(TFT_BLACK);
          tft.setTextSize(2);
          tft.drawString("Vazio",20,200);
    }  else if (a3s.diasAgenda == 5) {
              String hoursd= "";
              if(a3s.horaAgenda<10)
              {
                 hoursd = hoursd+"0"+String(a3s.horaAgenda);
                }else
              {
                hoursd = hoursd+""+String(a3s.horaAgenda);
              }
              if(a3s.minutoAgenda<10)
              {
                hoursd = hoursd+":0"+String(a3s.minutoAgenda);
              }else
              {
                hoursd = hoursd+":"+String(a3s.minutoAgenda);
              }
          tft.setTextColor(TFT_BLACK);
          tft.setTextSize(2);
          tft.drawString("Dias da Semana",20,200);
          tft.drawString(hoursd,213,200);
      
      }else if (a3s.diasAgenda == 6) {
              String hoursd= "";
              if(a3s.horaAgenda<10)
              {
                 hoursd = hoursd+"0"+String(a3s.horaAgenda);
                }else
              {
                hoursd = hoursd+""+String(a3s.horaAgenda);
              }
              if(a3s.minutoAgenda<10)
              {
                hoursd = hoursd+":0"+String(a3s.minutoAgenda);
              }else
              {
                hoursd = hoursd+":"+String(a3s.minutoAgenda);
              }
          tft.setTextColor(TFT_BLACK);
          tft.setTextSize(2);
          tft.drawString("Sábados",20,200);
          tft.drawString(hoursd,213,200);
      
      }else if (a3s.diasAgenda == 7) {
              String hoursd= "";
              if(a3s.horaAgenda<10)
              {
                 hoursd = hoursd+"0"+String(a3s.horaAgenda);
                }else
              {
                hoursd = hoursd+""+String(a3s.horaAgenda);
              }
              if(a3s.minutoAgenda<10)
              {
                hoursd = hoursd+":0"+String(a3s.minutoAgenda);
              }else
              {
                hoursd = hoursd+":"+String(a3s.minutoAgenda);
              }
          tft.setTextColor(TFT_BLACK);
          tft.setTextSize(2);
          tft.drawString("Domingos",20,200);
          tft.drawString(hoursd,213,200);
      
      }else if (a3s.diasAgenda == 8) {
              String hoursd= "";
              if(a3s.horaAgenda<10)
              {
                 hoursd = hoursd+"0"+String(a3s.horaAgenda);
                }else
              {
                hoursd = hoursd+""+String(a3s.horaAgenda);
              }
              if(a3s.minutoAgenda<10)
              {
                hoursd = hoursd+":0"+String(a3s.minutoAgenda);
              }else
              {
                hoursd = hoursd+":"+String(a3s.minutoAgenda);
              }
          tft.setTextColor(TFT_BLACK);
          tft.setTextSize(2);
          tft.drawString("Finais de semana",10,200);
          tft.drawString(hoursd,213,200);
      
      }
  

}

void  preparaCafe(int temps)
{

  int tempAgua=80;
  int botao = 2;
  bool pausar = 0;
  int i=0;


  
  for (i; i<15; i++){
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);


  
  mainMenu3(temps, tempC);
  digitalWrite(releRes,1);
  temps = temps - 1; 

//  Serial.println("Botão antes:");
//  Serial.println(botao);
  botao = checaBotao();

//  Serial.println("Botão Depois:");
//  Serial.println(botao);
  
  if (botao == 0) {
//    Serial.println("Botão 0:");
//    Serial.println(botao);
    digitalWrite(releRes, 0); //Desliga a Resistencia
    return;
  } 
//  else if (botao == 1) {
//    pausar = !pausar;
//    Serial.println("Botão 1:");
//    Serial.println(botao);
//    //   delay(200);
//  }

//  while (pausar == 1) {
//    Serial.println("Pausou");
//    digitalWrite(releRes, 1); //Desliga a Resistencia
////    digitalWrite(releAgua, 0);
//    delay(200);
//
//    Serial.println("Botão Antes da checagem, dentro do while:");
//    Serial.println(botao);
//    
//    botao = checaBotao();
//    Serial.println("Botão depois da checagem, dentro do while:");
//    Serial.println(botao);
//    if (botao == 0) {
//
//       Serial.println("Botão 0, dentro do while:");
//       Serial.println(botao);
//      digitalWrite(releRes, 0); //Desliga a Resistencia
// //     digitalWrite(releAgua, 0);
//      return;
//    } else if (botao == 1) {
//       Serial.println("Botão 1, dentro do while:");
//      Serial.println(botao);
//      pausar = !pausar;
//      digitalWrite(releRes, 1); //Desliga a Resistencia
//    }
//  }
  
   
   delay(130); //aguarda 1 Seg
  }

    
  
  while(temps > -1){
    
//  Serial.println(temps);

  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  mainMenu3(temps, tempC); 

  botao = checaBotao();
   if (botao == 0) {
//    Serial.println("Botão 0:");
//    Serial.println(botao);
    digitalWrite(releRes, 0); //Desliga a Resistencia
    return;
  }
  
//   
//   if (checaBotao()== 0)
//  { 
//    digitalWrite(releRes,0); //Desliga a Resistencia
//    digitalWrite(releAgua,0);
//    return;
//    } else if (checaBotao()== 1)
//    { 
//    pausar = !pausar;
// //   delay(200);
//    }
//  
//
//   while (pausar == 1){
//    Serial.println("Pausou");
//    delay(200);
//    if (checaBotao()== 0)
//      { 
//    digitalWrite(releRes,0); //Desliga a Resistencia
//    digitalWrite(releAgua,0);
//    return;
//    }    
//    if (checaBotao()== 1)
//                  { 
//    pausar = !pausar; 
//    }
//   }


 
//  digitalWrite(releRes,1); //Liga Resistencia
//  Serial.println("Ligou Resistencia");
  
  temps = temps - 1; 
  

  delay(130);
  
//  digitalWrite(releAgua,1); //Liga Aguá
//  Serial.println("Ligou Bomba dágua");
//
//  if(tempAgua < 80){
//      digitalWrite(releAgua,0);
//       for (int i=0; i<5; i++){
//        delay(1000); //aguarda 1 Seg
//       }
//      digitalWrite(releAgua,1);
//    }
  }
  
  
  
   digitalWrite(releRes,0); //Desliga a Resistencia
   digitalWrite(releAgua,0);
   preparoApp = 0;
   statusMenu = 1;
   return;
   
  
}

void  mainMenu6(int minn, int horan)
{

  uint16_t w = 0, h = 0, z= 0;
  //statusMenu = 6;
  
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

              
  TJpgDec.getJpgSize(&w, &h, menu6, sizeof(menu6));

  // Must use startWrite first so TFT chip select stays low during DMA and SPI channel settings remain configured
  tft.startWrite();

  // Draw the image, top left at 0,0 - DMA request is handled in the call-back tft_output() in this sketch
  TJpgDec.drawJpg(0, 0, menu6, sizeof(menu6));

  // Must use endWrite to release the TFT chip select and release the SPI channel
  tft.endWrite();

   tft.setTextColor(TFT_BLACK);
   tft.setTextSize(4);
   tft.drawString(timeChar2,60,150);

   


}
int checaBotao(){
    int pausar = 0;
    uint16_t xc, yc, zc;          
    tft.getTouchRaw(&xc, &yc);
    zc = tft.getTouchRawZ();

  
    if ((yc > 620) && (yc <1470)&& (zc > 400)) //verifica pause e cancela
                  {  
//                      if ((xc > 590) && (xc <1640)) //verifica o hora +
//                      {       
//                              if( pausar == 0){
//                                                pausar = 1;
//                                            //    Serial.println(pausar);
//                                                return 1;
//                                }
//                     
//                          }
//                          else if ((xc > 2100) && (xc <3360)) //verifica o minuto +
                        if ((xc > 2100) && (xc <3360)) //verifica o STOP
                          {  
                             statusMenu = 1;
                             preparoApp = 0;
                             mainMenu();
                      //       Serial.println("STOP");
                             delay (500);
                             return 0;
    
                          }
                  }
                  return 2;
                
}

int criaAgendamento( uint8_t  diasAgendaf, uint8_t  horaAgendaf,  uint8_t  minutoAgendaf,  int  quantidadeAgendaf, int  tempoAgendaf, struct Agendamentos *a1t, struct Agendamentos *a2t, struct Agendamentos *a3t){

   if (a1t->diasAgenda == 0 ){
      a1t->diasAgenda = diasAgendaf;
      a1t->horaAgenda = horaAgendaf;
      a1t->minutoAgenda = minutoAgendaf;
      a1t->quantidadeAgenda = quantidadeAgendaf;
      a1t->tempoAgenda = tempoAgendaf; 

      tft.fillScreen(TFT_WHITE);
      tft.setTextColor(TFT_BLACK);
      tft.setTextSize(2);
      tft.drawString("Agendamento Criado",40,120);
      delay(3000);

      return 0 ;
    }else if (a2t->diasAgenda == 0 ){
      a2t->diasAgenda = diasAgendaf;
      a2t->horaAgenda = horaAgendaf;
      a2t->minutoAgenda = minutoAgendaf;
      a2t->quantidadeAgenda = quantidadeAgendaf;
      a2t->tempoAgenda = tempoAgendaf; 
      tft.fillScreen(TFT_WHITE);
      tft.setTextColor(TFT_BLACK);
      tft.setTextSize(2);
      tft.drawString("Agendamento Criado",40,120);
      delay(3000);
      return 0;
      
    }else if(a3t->diasAgenda == 0 ){
      a3t->diasAgenda = diasAgendaf;
      a3t->horaAgenda = horaAgendaf;
      a3t->minutoAgenda = minutoAgendaf;
      a3t->quantidadeAgenda = quantidadeAgendaf;
      a3t->tempoAgenda = tempoAgendaf; 

      tft.fillScreen(TFT_WHITE);
      tft.setTextColor(TFT_BLACK);
      tft.setTextSize(2);
      tft.drawString("Agendamento Criado",50,120);
      delay(3000);
      
      return 0;
    }

    else     
      tft.fillScreen(TFT_WHITE);
      tft.setTextColor(TFT_BLACK);
      tft.setTextSize(2);
      tft.drawString("Agenda Cheia",80,120);
      return 1;
      delay(3000);
  
}

int verificaAlarme(struct Agendamentos a1t, struct Agendamentos a2t, struct Agendamentos a3t, int minuteNowf, int horaNowf, int diaNowf) {
  int dia = 0;
  int finde = 0;

  if (diaNowf == 1 || diaNowf == 2 || diaNowf == 3 || diaNowf == 4 || diaNowf == 5) {
    dia = 5;
  } else if (diaNowf == 0) {
    dia = 7;
    finde = 8;
  } else if (diaNowf == 6) {
    dia = 6;
    finde = 8;
  }

  
  if (a1t.diasAgenda != 0) {
//    Serial.println("Tem agendamento");
    if (a1t.diasAgenda == dia || a1t.diasAgenda == finde) {
//        Serial.println("É o dia");
//        Serial.println(a1t.horaAgenda);
//        Serial.println(horaNowf);
//        Serial.println(a1t.minutoAgenda);
//        Serial.println(minuteNowf);
        
      if (a1t.horaAgenda == horaNowf) {
//         Serial.println("É a hora");
        if (a1t.minutoAgenda == minuteNowf) {
//           Serial.println("É o minuto");
          return 1;
        }
      }
    }

  } 
  
  if (a2t.diasAgenda != 0) {
    if (a2t.diasAgenda == dia || a2t.diasAgenda == finde) {
      if (a2t.horaAgenda == horaNowf) {
        if (a2t.minutoAgenda == minuteNowf) {
          return 2;
        }
      }
    }

  } 
  
  if (a3t.diasAgenda != 0) {

    if (a3t.diasAgenda == dia || a3t.diasAgenda == finde) {

      if (a3t.horaAgenda == horaNowf) 
        if (a3t.minutoAgenda == minuteNowf) {
          return 3;
        }
      }
    }


  return 0;
}

String retornaAgendamentoApp (struct Agendamentos a1a, struct Agendamentos a2a, struct Agendamentos a3a){

     String agendaApp= "";
     
    if (a1a.diasAgenda == 0){
      agendaApp = agendaApp+String("Vazio,"); 
      } else {
               if(a1a.horaAgenda<10)
              {
                 agendaApp = agendaApp+"0"+String(a1a.horaAgenda);
                }else
              {
                agendaApp = agendaApp+""+String(a1a.horaAgenda);
              }
              if(a1a.minutoAgenda<10)
              {
                agendaApp = agendaApp+":0"+String(a1a.minutoAgenda);
              }else
              {
                agendaApp = agendaApp+":"+String(a1a.minutoAgenda);
              }
              if (a1a.diasAgenda == 5){
              agendaApp = agendaApp+String( " - Dias da Semana,");
              } else if (a1a.diasAgenda == 6){
              agendaApp = agendaApp+String( " - Sábados,");
              }else if (a1a.diasAgenda == 7){
              agendaApp = agendaApp+String( " - Domingos,");
              }else if (a1a.diasAgenda == 8){
              agendaApp = agendaApp+String( " - Finais de Semana,");
              }
      }
    if (a2a.diasAgenda == 0){
      agendaApp = agendaApp+String("Vazio,");
      }else {
               if(a2a.horaAgenda<10)
              {
                 agendaApp = agendaApp+"0"+String(a2a.horaAgenda);
                }else
              {
                agendaApp = agendaApp+""+String(a2a.horaAgenda);
              }
              if(a2a.minutoAgenda<10)
              {
                agendaApp = agendaApp+":0"+String(a2a.minutoAgenda);
              }else
              {
                agendaApp = agendaApp+":"+String(a2a.minutoAgenda);
              }
              if (a2a.diasAgenda == 5){
              agendaApp = agendaApp+String( " - Dias da Semana,");
              } else if (a2a.diasAgenda == 6){
              agendaApp = agendaApp+String( " - Sábados,");
              }else if (a2a.diasAgenda == 7){
              agendaApp = agendaApp+String( " - Domingos,");
              }else if (a2a.diasAgenda == 8){
              agendaApp = agendaApp+String( " - Finais de Semana,");
              }
      }


     if (a3a.diasAgenda == 0){
      agendaApp = agendaApp+String("Vazio");
      }else {
               if(a3a.horaAgenda<10)
              {
                 agendaApp = agendaApp+"0"+String(a3a.horaAgenda);
                }else
              {
                agendaApp = agendaApp+""+String(a3a.horaAgenda);
              }
              if(a3a.minutoAgenda<10)
              {
                agendaApp = agendaApp+":0"+String(a3a.minutoAgenda);
              }else
              {
                agendaApp = agendaApp+":"+String(a3a.minutoAgenda);
              }
              if (a3a.diasAgenda == 5){
              agendaApp = agendaApp+String( " - Dias da Semana,");
              } else if (a3a.diasAgenda == 6){
              agendaApp = agendaApp+String( " - Sábados,");
              }else if (a3a.diasAgenda == 7){
              agendaApp = agendaApp+String( " - Domingos,");
              }else if (a3a.diasAgenda == 8){
              agendaApp = agendaApp+String( " - Finais de Semana,");
              }
      }
 
  return agendaApp;
  }








  

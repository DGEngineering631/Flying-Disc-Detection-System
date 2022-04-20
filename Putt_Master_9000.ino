#include "HUSKYLENS.h"
#include "SoftwareSerial.h"

#define SHOCK_LED 12
#define SHOCK 13
// disc_state tracks sequence of operations
unsigned int disc_state = 0;
// basket radius in pixels
float basketradius = 80;
//basket center in pixels
float basketcenterX = -160;
float basketcenterY = -140;
//center of disc
float disc_locX = 0;
float disc_locY = 0;
float disc_loc = 0;
unsigned int i = 0;

HUSKYLENS huskylens;
SoftwareSerial mySerial(10, 11); // RX, TX
//HUSKYLENS green line >> Pin 10; blue line >> Pin 11
void printResult(HUSKYLENSResult result);

void setup() 
{
    pinMode(SHOCK, OUTPUT);
    pinMode(SHOCK_LED, OUTPUT);

    Serial.begin(115200);
    mySerial.begin(9600);
    while (!huskylens.begin(mySerial))
    {
        Serial.println(F("Begin failed!"));
        Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>Serial 9600)"));
        Serial.println(F("2.Please recheck the connection."));
    }
}

void loop() 
{
  i = 0;
  if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
  else if(!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
  else if(!huskylens.available()) Serial.println(F("No block or arrow appears on the screen!"));
  else  
    {
     if(huskylens.available())
      {
      Serial.println(F("Putt detected, wait for it to come to rest"));
      //disc has been detected on screen
      delay(1000); //pause to allow disc to come to rest
      Serial.println(F("3"));
      delay(1000);
      Serial.println(F("2"));
      delay(1000);
      Serial.println(F("1"));
      huskylens.request();
      }
      if(!huskylens.available())
       { 
        Serial.println(F("Putt Missed Off Screen"));
        digitalWrite(SHOCK,1);
        while (i < 2)
        {
        digitalWrite(SHOCK_LED,1);
        delay(200);
        digitalWrite(SHOCK_LED,0);
        delay(200);
        i = i + 1;
        }
        digitalWrite(SHOCK,0);
        i = 0;
       }
      else 
       { 
        Serial.println(F("Putt On Screen, checking location"));
        HUSKYLENSResult result = huskylens.read();
        disc_locX = float(result.xCenter) + basketcenterX;
        disc_locY = float(result.yCenter) + basketcenterY;
        disc_loc = sqrt(sq(float(disc_locX))+sq(float(disc_locY)));
        if(disc_loc > basketradius)
        {
          Serial.println(F("Putt missed, on screen, not in basket, ZAP!"));
          Serial.println(String(disc_locX));
          Serial.println(String(disc_locY));
          Serial.println(String(disc_loc));
          Serial.println(String()+F("Block:xCenter=")+result.xCenter+F(",yCenter=")+result.yCenter+F(",width=")+result.width+F(",height=")+result.height+F(",ID=")+result.ID);
          disc_state = 1; //Change disc state until disc is off screen, see while loop
          digitalWrite(SHOCK,1);
          while (i < 3)
            {
              digitalWrite(SHOCK_LED,1);
              delay(200);
              digitalWrite(SHOCK_LED,0);
              delay(100);
              i = i + 1;
            }
            digitalWrite(SHOCK,0);
            i = 0;
        }
        else 
        { 
         Serial.println(F("Nice Putt! Please Remove From Basket"));
         Serial.println(String(disc_locX));
         Serial.println(String(disc_locY));
         Serial.println(String(disc_loc));
         Serial.println(String()+F("Block:xCenter=")+result.xCenter+F(",yCenter=")+result.yCenter+F(",width=")+result.width+F(",height=")+result.height+F(",ID=")+result.ID);
         disc_state = 1;
         while (i < 4)
          {
          digitalWrite(SHOCK_LED,1);
          delay(1000);
          digitalWrite(SHOCK_LED,0);
          delay(500);
          i = i + 1;
          }
         i = 0;
        }
        while(disc_state == 1)
        {
          //Check if disc is removed to end loop
          huskylens.request();
          if(huskylens.available())
            {
            disc_state = 1;
            }
            else if(!huskylens.available())
            {
            disc_state = 0; //disc removed, now off screen, end loop and sequence
            Serial.println(F("Disc Removed"));
            while (i < 5)
              {
                digitalWrite(SHOCK_LED,1);
                delay(400);
                digitalWrite(SHOCK_LED,0);
                delay(400);
                i = i + 1;
              }
             i = 0;
            }
          }
        }
      }
    Serial.println(F("Try putting again!"));
  }

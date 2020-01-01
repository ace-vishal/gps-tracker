#include <SoftwareSerial.h>
#include <TinyGPS.h>

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
File myFile;

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
float latitude,longitude;
SoftwareSerial mySerial(10, 9); /* 10 to GPS Module TX, 09 to GPS Module RX  */
TinyGPS gps;
int temp=0,i;
void gpsdump(TinyGPS &gps);
void printFloat(double f, int digits = 2);
  long lat, lon;
  float flat, flon,gpsll;
  unsigned long age, date, time, chars;
  int CS = 8;


void setup()  
{
 
  lcd.begin(16,2);
  // Open serial communications and wait for port to open:
 
  
  Serial.begin(9600);
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
   
  lcd.print("Vehicle Tracking");
  lcd.setCursor(0,1);
  lcd.print("    System      ");
  
  delay(2000);

  gsm_init();
//sd();
  lcd.clear();
  Serial.println("AT+CNMI=2,2,0,0,0");
  lcd.print("GPS Initializing");
  lcd.setCursor(0,1);
  lcd.print("  No GPS Range  ");
  delay(2000);
  
    

    }

void loop() // run over and over to check incoming messages 
{  
 
  serialEvent();
  if(temp==1)
  {    
    tracking();
  }
      
delay(1000);


bool newdata = false;
  unsigned long start = millis();
  // Every 5 seconds we print an update
  while (millis() - start < 5000) 
  {
    if (mySerial.available()) 
    
    {
      char c = mySerial.read();
      //Serial.print(c);  // uncomment to see raw GPS data
      if (gps.encode(c)) 
      {
       newdata = true;
        break;         // uncomment to print new data immediately!
      }
    }
  }
  
   if (newdata) 
     {
       Serial.println("---");
      Serial.println();
      gpsdump(gps);                              
      Serial.println("---");   
     }  

 
}
void serialEvent()
{
  while(Serial.available())
  {
    if(Serial.find("Track"))
    {
      tracking();
      break;
    }
    else
    temp=0;
  }
}
void gsm_init()
{
 
      lcd.clear();
  lcd.print("Finding Module..");
  boolean at_flag=1;
  while(at_flag)
  {
    Serial.println("AT");
    while(Serial.available()>0)
    {
      if(Serial.find("OK"))
      at_flag=0;
    }
    
    delay(1000);
  }
  lcd.clear();
  lcd.print("Module Connected..");
  delay(1000);
  lcd.clear();
  lcd.print("Disabling ECHO");
  boolean echo_flag=1;
  while(echo_flag)
  {
    Serial.println("ATE0");
    while(Serial.available()>0)
    {
      if(Serial.find("OK"))
      echo_flag=0;
    }
    delay(1000);
  }
  lcd.clear();
  lcd.print("Echo OFF");
  delay(1000);
  lcd.clear();
  lcd.print("Finding Network..");
  boolean net_flag=1;
  while(net_flag)
  {
    Serial.println("AT+CPIN?");
    while(Serial.available()>0)
    {
      if(Serial.find("+CPIN: READY"))
      net_flag=0;
    }
    delay(1000);
  }
  lcd.clear();
  lcd.print("Network Found..");
  delay(1000);
  lcd.clear();
    
}

void gpsdump(TinyGPS &gps)
{
  long lat, lon;
  float flat, flon;
  unsigned long age, date, time, chars;

  gps.f_get_position(&flat, &flon, &age);
  Serial.print("Lat/Long(float): "); printFloat(flat, 5); Serial.print(", "); printFloat(flon, 5);
    Serial.print(" Fix age: "); Serial.print(age); Serial.println("ms.");
    
     lcd.clear();
     /* 
  lcd.print("SD card...");

    if(!SD.begin(CS))
  {
    Serial.println("Card Failure");
    return;
  }
 
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("gpslog.csv", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
 
  
 myFile.println(flat,6);
 myFile.print(flon,6);
 myFile.print("");
 

    // close the file:
    myFile.close();
    lcd.clear();
    lcd.println("done.");
  } else {
    // if the file didn't open, print an error:
    lcd.clear();
   lcd.println("error");
  }

  // re-open the file for reading:
  */
  
lcd.print("Long:");
lcd.print(flon,5);
lcd.setCursor(0,1);
lcd.print("Lat:");
lcd.print(flat,5);
 longitude = flon;
latitude = flat;
}

void printFloat(double number, int digits)
{
  // Handle negative numbers
  if (number < 0.0) 
  {
     Serial.print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  Serial.print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    Serial.print("."); 

  // Extract digits from the remainder one at a time
  while (digits-- > 0) 
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    Serial.print(toPrint);
    remainder -= toPrint;
  }
}

void tracking()
{
  Serial.println("AT+CMGF=1");
  delay(400);
  Serial.println("AT+CMGS=\"+917892699439\"");   // use your 10 digit cell no. here
  delay(1000);
  Serial.println("Vehicle Tracking Alert:");
  Serial.print("latitude");
  Serial.print(latitude,6);
  Serial.print("longitude");
  Serial.print(longitude,6);
  Serial.println("Click to locate now ");
  Serial.println ("http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=");
  Serial.print(latitude,6); Serial.print(longitude,6);
  delay(100);
  Serial.write(26);// ASCII code of CTRL+Z
  delay(1000);
  lcd.clear();
  lcd.print("Message Sent");
  delay(2000);
  lcd.clear();
  lcd.print("System Ready");
  //return;

}



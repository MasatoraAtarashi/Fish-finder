 #include "U8glib.h"
 U8GLIB_SSD1306_128X64 u8g(13, 11, 10, 9, 8);  // SW SPI Com: SCK = 13, MOSI = 11, CS = 10, A0 = 9

#include <SoftwareSerial.h>
SoftwareSerial mySerial =  SoftwareSerial(2,3); 

unsigned int reading;
byte readByte;
byte read_buffer[4]; 
byte crcCalc;
float distance;
float dst;
int measure_count;
float dsts[100];
float fish_depth;


 void setup() {
     u8g.setColorIndex(1);
     pinMode(13, OUTPUT);           
     digitalWrite(13, HIGH);
     
     mySerial.begin (9600);
     Serial.begin (9600);

     for (byte loopstep = 0; loopstep <= 3; loopstep++) {
        read_buffer[loopstep] = 0;
      }
 }

 void measure() {
    if (mySerial.available() < 1) {
      return;  
    }
    readByte = mySerial.read();
    for (byte loopstep = 0; loopstep <= 2; loopstep++) {
        read_buffer[loopstep] = read_buffer[loopstep + 01];
      }
      read_buffer[03] = readByte;  
      if (read_buffer[00] != 0xff) {
        return; // this is not the beginning of the data
      };
      crcCalc = read_buffer[00] + read_buffer[01] + read_buffer[02];
      if (read_buffer[03] != crcCalc) {
        return; // the checksum of the data packet did not match
      };
      distance = (read_buffer[01] * 0xff) + read_buffer[02];
      dst = distance/10.0;
      Serial.print(dst,1);
      Serial.println(" cm");
      measure_count++;
      dsts[measure_count - 1] = dst;
      delay(500);
      while (mySerial.available() > 0) {
         readByte = mySerial.read();
      }
 }
 void loop() {
    u8g.firstPage();  
     do {
      measure();
      if(measure_count >= 2) {
        u8g.setFont(u8g_font_unifont);
        u8g.setPrintPos(10, 10);
        u8g.print("depth: ");
        u8g.print(dsts[measure_count - 2],1);
        u8g.print("cm");
        if((dsts[measure_count - 2] - dsts[measure_count - 1]) > 5) {
          fish_depth = dsts[measure_count - 2] - dsts[measure_count - 1];
          u8g.setPrintPos(10, 40);
          u8g.print("fish: ");
          u8g.print(fish_depth,1);
          u8g.print("cm");
        } else {
          u8g.setPrintPos(10, 40);
          u8g.print("fish: ");
          u8g.print("none");
        }
      }
    } while( u8g.nextPage() );
 }

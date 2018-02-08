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
float depth;
float average;


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
      u8g.setFont(u8g_font_unifont);
      if(measure_count < 5) {
        u8g.setPrintPos(10, 10);
        u8g.print("depth: ");
        u8g.print("...");
        u8g.setPrintPos(10, 40);
        u8g.print("fish: ");
        u8g.print("...");
      }
      if(measure_count == 5 && depth == 0) {
          float sum = dsts[measure_count - 5] + dsts[measure_count - 4] + dsts[measure_count - 3] + dsts[measure_count - 2] + dsts[measure_count - 1];
          average = sum / 5;
          boolean error = true;
          int i = 5;
          while(i > 0) {
            if(dsts[measure_count - i] - average > 5 && dsts[measure_count - i] - average < -5) {
              error = false;
            }
            i--;
          }
          if(error) {
            depth = average;
          } else {
            measure_count = 0;
          }
      }
      if(measure_count > 5) {
        u8g.setPrintPos(10, 10);
        u8g.print("depth: ");
        u8g.print(depth,1);
        u8g.print("cm");
        if((depth - dsts[measure_count - 1]) > 5) {
          fish_depth = depth - dsts[measure_count - 1];
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

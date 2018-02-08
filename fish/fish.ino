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
float dsts[5];
int measure_count = 1;
float depth;
float average;
boolean depth_state = false;  //深さが設定済みならtrue
boolean fish_state = false; //魚がいたらtrue

 void setup() {
     u8g.setColorIndex(1);
     pinMode(13, OUTPUT);           
     digitalWrite(13, HIGH);
     
     mySerial.begin (9600);
     Serial.begin (9600);

     for (byte loopstep = 0; loopstep <= 3; loopstep++) {
        read_buffer[loopstep] = 0;
      }

     pinMode(4, INPUT);
 }

// 水深を測るメソッド
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
      dsts[measure_count - 1] = dst;
      measure_count++;
      delay(500);
      while (mySerial.available() > 0) {
         readByte = mySerial.read();
      }
 }
 void loop() {
    u8g.firstPage();  
    u8g.setFont(u8g_font_unifont);
     do {
      measure();
      if(depth_state == false) {
        Serial.println("false");
        setDepth();
      } else {
        Serial.println("true");
        u8g.setPrintPos(10, 10);
        u8g.print("depth: ");
        u8g.print(depth,1);
        u8g.print("cm");
        check();
        if(fish_state) {
          u8g.setPrintPos(10, 40);
          u8g.print("fish: ");
          u8g.print(dst,1);
          u8g.print("cm");
        } else {
          u8g.setPrintPos(10, 40);
          u8g.print("fish: ");
          u8g.print("none");
        }
      }
      if(digitalRead(4) == HIGH) {
        reset();
      }
    } while( u8g.nextPage() );
 }

// リセットボタンを押した時の処理
void reset() {
  Serial.println("reset");
  depth_state = false;
  setDepth();
}

//　水深を設定するメソッド
void setDepth() {
  Serial.println("setdepth");
  u8g.setFont(u8g_font_unifont);
  u8g.setPrintPos(10, 10);
  u8g.print("measuring...");
  if(measure_count == 5) {
    Serial.println("depth");
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
      measure_count = 1;
      depth_state = true;
    } else {
      measure_count = 1;
      depth_state = false;
    }
  }
}

//　魚がいないかチェックするメソッド
void check() {
  if(depth - dst > 5 || depth - dst < -5) {
    fish_state = true;
  } else {
    fish_state = false;
  }
}


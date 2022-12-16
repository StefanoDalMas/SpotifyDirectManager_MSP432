#include <Arduino_JSON.h>
#include "WiFi.h"
#include "HTTPClient.h"


#define RX 16
#define TX 17

const int baud_rate = 115200;
String value;

void setup() {
  Serial.begin(baud_rate);
  Serial2.begin(baud_rate,SERIAL_8N1,RX,TX); //define serial channel
  Serial.println("Setup completed :P");
  value = String("");
}
bool acked = false;
char chr;


void readdata(){
  while(Serial2.available()){
      chr = Serial2.read();
      if(chr >='a' && chr <= 'z'){
        value.concat(chr);
      }
      Serial.print("La stringa atm è ");
      Serial.println(value);
      if (value.compareTo("prev") == 0){
        Serial.println("HO VISTOOOOOO prev\n\n");
        value = String("");
        Serial2.write('%');
      }
      else if (value.compareTo("next") == 0){
        Serial.println("HO VISTOOOOOO next\n\n");
        value = String("");
        Serial2.write('%');
      }
      else if (value.compareTo("upup") == 0){
        Serial.println("HO VISTOOOOOO upup\n\n");
        value = String("");
        Serial2.write('%');
      }
      else if (value.compareTo("down") == 0){
        Serial.println("HO VISTOOOOOO down\n\n");
        value = String("");
        Serial2.write('%');
      }
      else if (value.compareTo("play") == 0){
        Serial.println("HO VISTOOOOOO play\n\n");
        value = String("");
        Serial2.write('%');
      }
      else if (value.compareTo("stop") == 0){
        Serial.println("HO VISTOOOOOO stop\n\n");
        value = String(""); 
        Serial2.write('%');
      }
      
      
    }
    value = String("");
    
}

void loop() {
  readdata();
  // SCRIVE
  while(Serial.available()){
    chr = Serial.read();
    if (chr == 't'){
      Serial2.write('c');
      while(Serial2.available() && !acked){
        char ack = Serial2.read();
        if (ack == '%'){
          acked = true;
        }
      }
      Serial2.write('i');

      
      Serial.println("Data sent");  
      acked = false;
    }
  }
}
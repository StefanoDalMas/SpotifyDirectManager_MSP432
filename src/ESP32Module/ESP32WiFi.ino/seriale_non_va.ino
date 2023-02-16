#include <Arduino_JSON.h>
#include "WiFi.h"
#include "HTTPClient.h"


#define RX 16 //goes to 3.3
#define TX 17 //goes to 3.2

const char* ssid = "iPhone di Stefano"; //insert here
const char* password = "oo8uki1diy6jy"; //insert here
const char* token = "Bearer "; //remember to create and add token
const char* spotify_base_url = "https://api.spotify.com/v1/me";
const int baud_rate = 115200;


bool acked = false;
char chr;
char jsonOutput[128];
String value;
enum mode{
  GET, POST, PUT
};




void setup() {
  Serial.begin(baud_rate);
  Serial2.begin(baud_rate,SERIAL_8N1,RX,TX); //define serial channel
  //Disconnect from previous AP
  /*
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  scanWiFi(); //checks WiFi APs available
  initWiFi(); //connect to desired AP
  value = String("");
  */
  Serial.println("Setup completed :P");
  //Serial.println("Press g to perform GET, p to perform \"pause\" and n to perform \"next\" \n"); andy frocio
}

void loop() {
  //OLD 

    // performHTTP("/player/devices",GET);
    // performHTTP("/player/volume?volume_percent=70",PUT);
    // performHTTP("/player/pause",PUT);
    // performHTTP("/player/next",POST);
  //NEW 
  readdata();
  // SCRIVE
  /*
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
  */
}

/* WIFI RELATED
void performHTTP(String url, mode m){
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    String base_url = spotify_base_url;
    base_url.concat(url);
    http.begin(base_url);
    http.addHeader("Authorization",token);
    http.addHeader("Content-Type", "application/json");

    int httpcode;  
    switch (m) {
      case GET:
        httpcode = http.GET();
        break;
      case POST:
        http.addHeader("Content-Length","0");
        httpcode= http.POST("");
        break;
      case PUT:
        http.addHeader("Content-Length","0");  
        httpcode= http.PUT("");
        break;
      default:
        Serial.println("Mode not recognized");
        break;
    }
    if ((httpcode > 199)&&(httpcode < 300)){
      if(m == GET){
        String val = http.getString();
        parse_print_request(val);
      }
    }
    else{
      Serial.print("HTTP ERROR VALUE IS ");
      Serial.println(httpcode);
    }
    http.end();
  }
  else{
    Serial.println("Not connected to WiFi :(");
  }
}

void parse_print_request(String val){
  JSONVar myObject = JSON.parse(val); //convert to JSONObject
  //Serial.println(myObject);
  JSONVar keys = myObject.keys(); //Get alla keys
  //Print all data
  for(int i=0;i<keys.length();i++){
    JSONVar value = myObject[keys[i]]; //get all values
    Serial.print(keys[i]);
    Serial.print(" = ");
    Serial.println(value);

  }
}

void scanWiFi(){
  Serial.println("Scanning...");
  int n = WiFi.scanNetworks();
  if (n == 0){
    Serial.println("No networks found");
  }
  else{
    Serial.println("Network found");
    //Print all APs
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":" encrypted");
      delay(10);
    }
  }
  Serial.println("");
}

void initWiFi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.print("\n Connected to AP ");
  Serial.println(WiFi.localIP());  
}
*/


void readdata(){
  while(Serial2.available()){
      chr = Serial2.read();
      if(chr >='a' && chr <= 'z'){
        value.concat(chr);
      }
      if (strcmp(value.c_str(), "") != 0){
        Serial.print("La stringa atm è ");
      Serial.println(value);
      }
      
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

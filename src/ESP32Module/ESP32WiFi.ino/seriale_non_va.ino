#include <Arduino_JSON.h>
#include "WiFi.h"
#include "HTTPClient.h"


#define RX 16 //goes to 3.3
#define TX 17 //goes to 3.2
#define INTERNET_ACTIVE 0
//WHACKY PROTOCOL
//10-19 volume + playing
//20-29 volume + not playing
// % is 37, # is 35

const char* ssid = "iPhone di Stefano"; //insert here
const char* password = "oo8uki1diy6jy"; //insert here
const char* token = "Bearer BQCiVhkm3-T1jo3SY7Rqqk1JA_LM0U0UCZBSRoHC7WexC8xuOABw6m29KPtzXEMg7TGQbU-rfQ3w84QmxYxbZ5vREPvJOTqp7GBdUV_fIg9x0WmSgVOShjbWvi21FEeUufCieMmBRCNTFxMQgq6fT7OxQC4-owWzA1OipDW868oTtGyOY_qpJEDA9pxDY_vkMEpnyg"; //remember to create and add token
const char* spotify_base_url = "https://api.spotify.com/v1/me";
const int baud_rate = 115200;

bool acked = false;
int volume = 50;
bool playing = false;
char chr;
char jsonOutput[128];
String value;

enum mode{
  GET, POST, PUT
};
void performHTTP(String url, mode m);




void setup() {
  Serial.begin(baud_rate);
  Serial2.begin(baud_rate,SERIAL_8N1,RX,TX); //define serial channel
  //Disconnect from previous AP
  if (INTERNET_ACTIVE){
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    scanWiFi(); //checks WiFi APs available
    initWiFi(); //connect to desired AP
    value = String("");
    performHTTP("/player/volume?volume_percent=50",PUT);
    performHTTP("/player/pause",PUT);
  }
  Serial.println("Setup completed :P");
  //Serial.println("Press g to perform GET, p to perform \"pause\" and n to perform \"next\" \n"); andy frocio
}

void loop() {
  while(Serial2.available()){
    value = Serial2.readStringUntil('\0');
    if (value.compareTo("") != 0){
      if (value.compareTo("prev") == 0){
        if (INTERNET_ACTIVE){
          performHTTP("/player/previous",POST); 
        }
        Serial.println("HO VISTOOOOOO prev\n\n");
        value = String("");
        Serial2.write('%');
      }
      else if (value.compareTo("next") == 0){
        if (INTERNET_ACTIVE){
          performHTTP("/player/next",POST);          
        }
        Serial.println("HO VISTOOOOOO next\n\n");
        value = String("");
        Serial2.write('%');
      }
      else if (value.compareTo("upup") == 0){
        if (volume <100){
          volume +=10;
          String set = "/player/volume?volume_percent=";
          set.concat(volume);
          if(INTERNET_ACTIVE){
            performHTTP(set,PUT);
          }          
        }
        Serial.println("HO VISTOOOOOO upup\n\n");
        value = String("");
        Serial2.write('%');
      }
      else if (value.compareTo("down") == 0){
        if (volume > 0){
          volume -= 10;
          String set = "/player/volume?volume_percent=";
          set.concat(volume);
          if (INTERNET_ACTIVE){
            performHTTP(set,PUT);
          }
        }
        Serial.println("HO VISTOOOOOO down\n\n");
        value = String("");
        Serial2.write('%');
      }
      else if (value.compareTo("play") == 0){
        if (INTERNET_ACTIVE){
          performHTTP("/player/play",PUT);
        }
        Serial.println("HO VISTOOOOOO play\n\n");
        value = String("");
        Serial2.write('%');
      }
      else if (value.compareTo("stop") == 0){
        if (INTERNET_ACTIVE){
        performHTTP("/player/pause",PUT);
        }
        Serial.println("HO VISTOOOOOO stop\n\n");
        value = String(""); 
        Serial2.write('%');
      }  
    }
  }
    value = String("");
}



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

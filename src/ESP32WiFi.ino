#include <ArduinoJson.h>
#include "WiFi.h"
#include <Arduino_JSON.h>
#include "HTTPClient.h"

#define RX 16 //goes to 3.3
#define TX 17 //goes to 3.2
#define INTERNET_ACTIVE 1

const char* ssid = "iPhone di Stefano"; //insert here
const char* password = "oo8uki1diy6jy"; //insert here
// togliere il token
const char* token = "Bearer BQCEB7Lnv0Wu4JwS_l9OctnytXY1TQ_bWY7EaY62jMy55gD3xkYrMr7q1pe7UeFNIDxH55RJZgz7FU9ji18z6oiAIri0Qc0kQin_AjKwzceTf0cXnHnEHl4-IIsuCG-Gw1Tt5ZRzQLwVl-u4HBFZteEyZOzgGa2xG5xtG5QPDWSRco8-HxjjcgSVKOLrWxhjsqT7Bw"; //remember to create and add token
const char* spotify_base_url = "https://api.spotify.com/v1/me";
const int baud_rate = 115200;

bool acked = false;
int volume = 50;
bool playing = false;
char chr;
char jsonOutput[128];
String value;

int timer =1;
char c;
String send;
String songName;
String artistName;

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
  Serial.println("Setup completed");
}

void loop() {
  while(Serial2.available()){
    value = Serial2.readStringUntil('\0');
    if (value.compareTo("") != 0){
      if (value.compareTo("prev") == 0){
        if (INTERNET_ACTIVE){
          performHTTP("/player/previous",POST); 
        }
        performHTTP("/player/currently-playing", GET);
        artistName.concat('#');
        Serial.println(artistName);
        sendString(artistName);
        //now the songname
        delay(100); //make sure msp received and copied the string
        songName.concat('#');
        sendString(songName);
        artistName = String("");
        songName = String("");
        value = String("");
      }
      else if (value.compareTo("next") == 0){
        if (INTERNET_ACTIVE){
          performHTTP("/player/next",POST);          
        }
        //Authorname
        performHTTP("/player/currently-playing", GET);
        artistName.concat('#');
        sendString(artistName);
        //now the songname
        delay(100); //make sure msp received and copied the string
        songName.concat('#');
        sendString(songName);
        artistName = String("");
        songName = String("");
        value = String("");
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
        value = String("");
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
        value = String("");
      }
      else if (value.compareTo("play") == 0){
        if (INTERNET_ACTIVE){
          performHTTP("/player/play",PUT);
        }
        value = String("");
      }
      else if (value.compareTo("stop") == 0){
        if (INTERNET_ACTIVE){
        performHTTP("/player/pause",PUT);
        }
        value = String("");
      }  
    }
  }
    value = String("");
}

void sendString(String send){
  for(int i=0; i<send.length();i++){
    c = send.charAt(i);
    Serial2.write(c);
    timer--;
    if (timer == 0 ){
      String tmp = Serial2.readStringUntil('%');
      Serial.println(tmp);
      timer = 1;
    }
  }
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
    Serial.println("Not connected to WiFi");
  }
}
void parse_print_request(String val){
  DynamicJsonDocument doc(16384);
  DeserializationError error = deserializeJson(doc, val);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.f_str());
    return;
  }

  // Print the song name and artist name
  songName = doc["item"]["name"].as<String>();
  artistName = doc["item"]["artists"][0]["name"].as<String>();
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

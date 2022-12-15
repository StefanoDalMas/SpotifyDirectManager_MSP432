#include <Arduino_JSON.h>
#include "WiFi.h"
#include "HTTPClient.h"

const char* ssid = "iPhone di Stefano"; //insert here
const char* password = "oo8uki1diy6jy"; //insert here
//Spotify Stuff
//You always have to generate a new token and place it after the space character in "token".
//If you get ERROR 401 it means that your token expired
const char* token = "Bearer "; //remember to create and add token
const char* spotify_base_url = "https://api.spotify.com/v1/me";

const int baud_rate = 115200;

char jsonOutput[128];

enum mode{
  GET, POST, PUT
};

void setup() {
  Serial.begin(baud_rate);
  Serial.println("Disconnecting from previous AP...");
  //Disconnect from previous AP
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  scanWiFi(); //checks WiFi APs available
  initWiFi(); //connect to desired AP
  Serial.println("Setup completed :)");
  Serial.println("Press g to perform GET, p to perform \"pause\" and n to perform \"next\" \n");
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

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial.available()){
    char c;
    c= Serial.read();
    if (c == 'g'){
      Serial.println("Performing GET ");
      performHTTP("/player/devices",GET);
      Serial.println("GET Performed");
    }
    if(c == 'p'){
      Serial.println("Performing pause ");
      performHTTP("/player/pause",PUT);
      Serial.println("pause Performed");
    }
    if (c == 'n'){
      Serial.println("Performing next ");
      performHTTP("/player/next",POST);
      Serial.println("next Performed");
    }
    if (c == 'v'){
      Serial.println("Performing volume change ");
      performHTTP("/player/volume?volume_percent=70",PUT);
      Serial.println("Volume changed");
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
    Serial.println("Not connected to WiFi :(");
  }
}
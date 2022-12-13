#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

#include <Arduino_JSON.h>

#include "WiFi.h"

#include "HTTPClient.h"



const char* ssid = "iPhone di Stefano"; //insert here
const char* password = "oo8uki1diy6jy"; //insert here
const char* serverpost = "http://jsonplaceholder.typicode.com/posts";
const char* serverget = "https://dummyjson.com/products/1";

//Spotify Stuff
const char* token = "Bearer "; //remember to create and add token
const char* spotify_base_url = "https://api.spotify.com/v1/me";



char jsonOutput[128];


unsigned long lastTime = 0;
unsigned long timerDelay = 5000; //5 sec




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Disconnecting from previous AP...");
  //Disconnect from previous AP
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  scanWiFi(); //checks WiFi APs available
  initWiFi(); //connect to desired AP
  Serial.println("Setup completed :)");
  Serial.println("Press g to perform GET, p to perform POST\n");
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
    char c = Serial.read();
    if (c == 'g'){
      Serial.println("Performing GET ");
      getPage();
    }
    if(c == 'p'){
      Serial.println("Performing POST ");
      postPage();
    }

  }
}


void getPage(){
  if(WiFi.status()== WL_CONNECTED)
  {
    HTTPClient http;
    String get_url = spotify_base_url;
    get_url.concat("/player/devices");
    http.begin(get_url);
    http.addHeader("Authorization", token);
    http.addHeader("Content-Type", "application/json");

    Serial.println(get_url);
    //Catch response
    int response = http.GET();
    Serial.println("HTTP code: ");
    Serial.println(response);
    if (response > 0){
      if (response == HTTP_CODE_OK){ //It's 200
          String val = http.getString();
          parse_print_request(val);
      }
    }
    else{
      Serial.println("HTTP Failed: error code is ");
      Serial.println(http.errorToString(response).c_str()); //convert String into arryas of char with '\0' at the end
    }

    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}

void postPage(){
  if(WiFi.status()== WL_CONNECTED)
  {
    HTTPClient http;
    String post_url = spotify_base_url;
    post_url.concat("/player/next");
    http.begin(post_url);
    http.addHeader("Authorization",token);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Content-Length","0");
    Serial.println(post_url);


    //useless stuff kinda
    // const size_t CAPACITY = JSON_OBJECT_SIZE(1);
    // StaticJsonDocument<CAPACITY> doc;
    //object["title"] = "Vediamo se sto giro funziona";

    // JsonObject object = doc.to<JsonObject>();

    //serializeJson(doc,jsonOutput); 
    int httpcode = http.POST("");
    if (httpcode > 0){
      Serial.println("Ok \n");
      Serial.println(httpcode);
      Serial.println("\n");

      String res = http.getString();
      Serial.println(res);
    }else{
      Serial.println("Error code in sending data");
    }

    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}

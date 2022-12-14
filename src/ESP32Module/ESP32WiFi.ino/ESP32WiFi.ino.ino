#include <Arduino_JSON.h>
#include "WiFi.h"
#include "HTTPClient.h"

const char* ssid = "iPhone di Stefano"; //insert here
const char* password = "oo8uki1diy6jy"; //insert here
//Spotify Stuff
//You always have to generate a new token and place it after the space character in "token".
//If you get ERROR 401 it means that your token expired
const char* token = "Bearer BQCZaaW9vFa0duHz9J7IdvnD-pMb6pHItY034gFieEu6e6t0tPAaQUHb1kUIdkmd_QGoJ-R_5MYL_64UXs6uA6FyUmSUEzrMPK8RV0WTdQfpOfBRKaEQ5dSydcN__ZY7e6HwTNqBLDAZzdSY2vgMwuzrZBZ71LRwGqKj0G4TKmoz4hzQGhSyQyBE-vadgila0Y8j"; //remember to create and add token
const char* spotify_base_url = "https://api.spotify.com/v1/me";

char jsonOutput[128];

enum mode{
  GET, POST, PUT
};

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
    char c = Serial.read();
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

// void performHTTP(){
//   if(WiFi.status() == WL_CONNECTED){    
//   }
//   else{
//     Serial.println("WiFi Disconnected");
//   }
// }

// void putPage(){
//   if(WiFi.status() == WL_CONNECTED){
//     HTTPClient http;
//     String put_url = spotify_base_url;
//     put_url.concat("/player/pause");   
//     http.begin(put_url);
//     http.addHeader("Authorization",token);
//     http.addHeader("Content-Type", "application/json");
//     http.addHeader("Content-Length","0");
//     Serial.println(put_url);
//     int httpcode = http.PUT("");    
//     if (httpcode > 0){
//       Serial.println("HTTP Response");
//       Serial.println(httpcode);
//     }
//     else{
//       Serial.println("EWrrore");
//     }
    
//     http.end(); 

//   }
//   else{
//     Serial.println("Bruh è disconnected");
//   }

// }

// void getPage(){
//   if(WiFi.status()== WL_CONNECTED)
//   {
//     HTTPClient http;
//     String get_url = spotify_base_url;
//     get_url.concat("/player/devices");
//     http.begin(get_url);
//     http.addHeader("Authorization", token);
//     http.addHeader("Content-Type", "application/json");

//     Serial.println(get_url);
//     //Catch response
//     int response = http.GET();
//     Serial.println("HTTP code: ");
//     Serial.println(response);
//     if (response > 0){
//       if (response == HTTP_CODE_OK){ //It's 200
//           String val = http.getString();
//           parse_print_request(val);
//       }
//     }
//     else{
//       Serial.println("HTTP Failed: error code is ");
//       Serial.println(http.errorToString(response).c_str()); //convert String into arryas of char with '\0' at the end
//     }

//     http.end();
//   }
//   else {
//     Serial.println("WiFi Disconnected");
//   }
// }

// void postPage(){
//   if(WiFi.status()== WL_CONNECTED)
//   {
//     HTTPClient http;
//     String post_url = spotify_base_url;
//     post_url.concat("/player/next");
//     http.begin(post_url);
//     http.addHeader("Authorization",token);
//     http.addHeader("Content-Type", "application/json");
//     http.addHeader("Content-Length","0");
//     Serial.println(post_url);


//     //useless stuff kinda
//     // const size_t CAPACITY = JSON_OBJECT_SIZE(1);
//     // StaticJsonDocument<CAPACITY> doc;
//     //object["title"] = "Vediamo se sto giro funziona";

//     // JsonObject object = doc.to<JsonObject>();

//     //serializeJson(doc,jsonOutput); 
//     int httpcode = http.POST("");
//     if (httpcode > 0){
//       Serial.println("Ok \n");
//       Serial.println(httpcode);
//       Serial.println("\n");
//     }else{
//       Serial.println("Error code in sending data");
//     }

//     http.end();
//   }
//   else {
//     Serial.println("WiFi Disconnected");
//   }
// }

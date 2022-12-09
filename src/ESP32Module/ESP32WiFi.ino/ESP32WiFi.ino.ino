#include "WiFi.h"
#include "Arduino_JSON.h"
#include "HTTPClient.h"


const char* ssid = ""; //insert here
const char* password = ""; //insert here
const char* server = "https://cazza.free.beeceptor.com/getcazza";


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

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial.available()){
    char c = Serial.read();
    if (c == 's'){
      Serial.println("Received start symbol");
      getPage();
    }
  }
}


void getPage(){
  if(WiFi.status()== WL_CONNECTED)
  {
    HTTPClient http;
    http.begin(server);
    Serial.println(server);
    //Catch response
    int response = http.GET();
    Serial.println("HTTP code: ");
    Serial.println(response);
    if (response > 0){
      if (response == HTTP_CODE_OK){ //It's 200
          String val = http.getString();
          Serial.println(val);
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

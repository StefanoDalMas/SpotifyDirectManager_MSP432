#include "WiFi.h"

const char* ssid = "iPhone di Stefano";
const char* password = "oo8uki1diy6jy";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Disconnecting from previous AP...");
  //Disconnect from previous AP
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

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
  Serial.print("Connected to AP")
  Serial.println(WiFi.localIP());  
}

void loop() {
  // put your main code here, to run repeatedly:
  scanWiFi();
  initWiFi();
  while(){}

}
  

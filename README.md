# SpotifyDirectManager_MSP432
Project for Embedded Software for the IoT

## Description
This project is a simple implementation of a Spotify Controller. Our project is based on the MSP432P401R Launchpad.
Our controller is able to play, pause, skip, and go back to the previous song. It is also able to change the volume of the song. The controller is able to do this by using the buttons on the Launchpad. The controller is also able to display the song name, artist, and album cover on the LCD screen. The controller is able to do this by using the Spotify APIs.
It exploits the Spotify APIs to get the song name, artist, and album cover. It then uses the UART protocol to send the data to the MSP432 which then displays the data on the LCD screen.
The ESP connects to the wifi using the wifi library and the http client library. It then uses the UART protocol to communicate with the MSP432. With the tokens we call the spotify APIs using the HTTP client library. We then parse the JSON data. We then send the data to the MSP using the UART protocol.

## Requirements
### Hardware
1. MSP432P401R Launchpad
2. MSP432P401R BoosterPack
3. ESP32

### Software
1. Code Composer Studio
2. Arduino IDE

### External libraries and APIs
1. Driverlib
2. Grlib
3. Spotify APIs
4. Arduino_JSON
5. WiFi
6. HTTPClient

## Installation
### Hardware
1. Connect the Boosterpack to the MSP432
2. Connect the MSP432 to the computer
3. Connect the ES32 to the computer using the USB cable
4. Connect the ESP32 to the MSP432
In order to set up the UART communication we need to the following prelimary steps:
1. Connect pin 16 of the ESP to pin 3.3 of the MSP
2. Connect pin 17 of the ESP to pin 3.2 of the MSP

### Software
1. Clone the repository
2. Open the project in Code Composer Studio, if you don't have it, you can download it [here](http://www.ti.com/tool/CCSTUDIO)
3. Build the project and burn it to the MSP432
4. Open the `ESP32WiFi.ino` file in the Arduino IDE
5. Burn the code to the ESP32
6. Download the [driverlib](https://www.ti.com/tool/MSPDRIVERLIB) and [grlib](https://www.ti.com/tool/MSP-GRLIB) libraries

### External libraries and APIS
#### MSP432
1. Download the [driverlib](https://www.ti.com/tool/MSPDRIVERLIB) and [grlib](https://www.ti.com/tool/MSP-GRLIB) libraries
2. Add the libraries to the linker file

#### ESP32
1. Install the following libraries from the Arduino IDE library manager:
    1. [Arduino_JSON]
    2. [WiFi]
    3. [HTTPClient]
2. Modify the credentials for internet access in the `ESP32WiFi.ino` file
3. Modify the bearer token as explained in the [next section](#spotify-apis)

#### Spotify APIS
The next steps are to be followed in order to get the access token:
1. Create a Spotify developer account
2. Create a new project
3. Add a [new application](https://developer.spotify.com/dashboard/login)
4. Set a new redirect URI for the application
5. Copy the following link on your browser 
```
(https://accounts.spotify.com/authorize?client_id={XXX}&response_type=code&scope={XXX}&redirect_uri={XXX})
```
The {XXX} need to be changed to the appropriate values:
    1. client_id: the client id of the application
    2. scope: the actions you want to perform in your application. e.g.
    user-read-playback-state user-modify-playback-state user-read-currently-playing
    3. redirect_uri: the redirect uri of the application (it has to be URL encoded)
6. Copy the token from the redict uri
7. Use the token to call the 'curl' command in your terminal
```
curl -H "Authorization: Basic {XXX}=" -d grant_type=authorization_code -d code={XXX} -d redirect_uri={XXX} https://accounts.spotify.com/api/token
```
The {XXX} need to be changed to the appropriate values:
    1. authorisation: client_id:client_secret (it has to be URL encoded)
    2. code: the token obtained from the redirect uri
    3. redirect_uri: the redirect uri of the application (it has to be URL encoded)
8. Copy the access token from the response
9. Append the access token to the `token` variable in the `ESP32WiFi.ino.ino` file so that it is "Bearer {appended_token}"

## Usage
1. Connect the MSP432 to the computer
2. Connect the ESP32 to the MSP432
3. Build and run the project

## Options
1. Play/Pause
2. Skip
3. Previous
4. Volume Up
5. Volume Down

## Info
1. Display Song Name
2. Display Artist
3. Play/pause state by spinning logo


## Credits
1. [Spotify APIs](https://developer.spotify.com/web-api/)
2. [MSP432P401R Launchpad](http://www.ti.com/tool/MSP-EXP432P401R)
3. [ESP32](https://www.espressif.com/en/products/hardware/ESP32ex/overview)

## Contact
1. [Stefano Dal Mas](stefano.dalmas@studenti.unitn.it)

2. [Andy Ion Ditu](andyion.ditu@studenti.unitn.it)

3. [Amir Gheser](amir.gheser@studenti.unitn.it)


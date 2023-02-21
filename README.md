# SpotifyDirectManager_MSP432
Project for Embedded Software for the IoT

## Description
Our project is a Spotify track controller. It allows the user to control the Spotify player on his/her smartphone from a remote device. The remote device is a MSP432P401R Launchpad with a BoosterPack which communicates with the smartphone via Spotify APIs. To do so, we connected our controller to a 2.4 GHz network. Communication between components of our device occurs via UART whereas communication with the Spotify APIs follows the HTTP protocol. Amongst the various features the user will be able to play/pause the song, skip to the next/previous song, change the volume, by using the buttons of the BoosterPack and see the current song name and artist. In addition, we implemented a feature that allows the user to change the volume by rapidly moving the controller up or down which is done thanks to the accelerometer along the Z axis. 

## Table of contents
1. [Description](#description)
2. [Table of contents](#table-of-contents)
3. [Project Structure](#project-structure)
4. [Requirements](#requirements)
    1. [Hardware](#hardware)
    2. [Software](#software)
    3. [External libraries and APIs](#external-libraries-and-apis)
5. [Installation](#installation)
    1. [Hardware](#hardware-1)
    2. [Software](#software-1)
    3. [External libraries and APIS](#external-libraries-and-apis-1)
        1. [MSP432](#msp432)
        2. [ESP32](#esp32)
        3. [Spotify APIS](#spotify-apis)
6. [Usage](#usage)
7. [Options](#options)
8. [Info](#info)
9. [Presentation and Demo](#presentation-and-demo)
10. [Contact](#contact)

## Project Structure
```
├───.vscode
├───src
│   ├───extra
│       |───automator.py #open through CLI connection with edgeImpulse
│       |───svm.py # SVM classifier
│   ├───images
│   ├───msp432
│   │   ├───grlib
│   │   └───driverlib
│   └───esp32
└───.gitignore
```

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
1. Connect the BoosterpackMKII to the MSP432
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

### External libraries and APIS
#### MSP432
1. Download the [driverlib](https://www.ti.com/tool/MSPDRIVERLIB) and [grlib](https://www.ti.com/tool/MSP-GRLIB) libraries
2. Add the libraries to the linker and loader in the project properties

#### ESP32
1. Install the following libraries from the Arduino IDE library manager:
    1. ArduinoJson
    2. WiFi
    3. HTTPClient
2. Modify the credentials for internet access in the `ESP32WiFi.ino` file
3. Modify the bearer token as explained in the [next section](#spotify-apis)

#### Spotify APIS
##### Connecting to the Spotify API
The next steps are to be followed in order to get the access token:
1. Create a Spotify developer account
2. Create a new project on the Spotify developer dashboard
3. Add a [new application](https://developer.spotify.com/dashboard/login)
4. Set a new redirect URI for the application
5. Copy the following link on your browser 
    ```
    (https://accounts.spotify.com/authorize?client_id={XXX}&response_type=code&scope={XXX}&redirect_uri={XXX})
    ```
    The {XXX} need to be changed to the appropriate values:
    1. client_id: the client id of the application
    2. scope: the actions you want to perform in your application. 
    For this project we need the following scopes:
        ```
        user-read-playback-state user-modify-playback-state user-read-currently-playing
        ```
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
9. Append the access token to the `token` variable in the `ESP32WiFi.ino` file so that it is "Bearer {appended_token}"

##### Using the APIs
The following table will show the APIs used in this project and the corresponding HTTP requests:
| API | HTTP Request | Description |
| --- | --- | --- |
| https://api.spotify.com/v1/me/player/currently-playing | GET | Get the current song |
| https://api.spotify.com/v1/me/player/play | PUT | Play the current song |
| https://api.spotify.com/v1/me/player/play | PUT | Pause the current song |
| https://api.spotify.com/v1/me/player/pause | PUT | Pause the current song |
| https://api.spotify.com/v1/me/player/next | POST | Skip the current song |
| https://api.spotify.com/v1/me/player/previous | POST | Play the previous song |
| https://api.spotify.com/v1/me/player/volume?volume_percent=50 | PUT | Change the volume        e.g. 50|

Volume can be changed from 0 to 100




## Usage
1. Connect the MSP432 to the computer as described in the [installation section](#installation)
2. Connect the ESP32 to the MSP432 as described in the [installation section](#installation)
3. [Build](#hardware-1) and run the project

1. If you haven't already install as described in the [installation section](#installation)
2. Run the project


## Options
<img title="Buttons" alt="Buttons" height="24" width="24" src="https://github.com/StefanoDalMas/SpotifyDirectManager_MSP432/blob/main/src/images/lb-guide.jpg?raw=true">

1. Play <img title="Play" alt="play" height="24" width="24" src="https://github.com/StefanoDalMas/SpotifyDirectManager_MSP432/blob/main/src/images/spotify-controls/play.png">
2. Pause <img title="Pause" alt="pause" height="24" width="24" src="https://github.com/StefanoDalMas/SpotifyDirectManager_MSP432/blob/main/src/images/spotify-controls/pause.png">
3. Next <img title="Next" alt="next" height="24" width="24" src="https://github.com/StefanoDalMas/SpotifyDirectManager_MSP432/blob/main/src/images/spotify-controls/next.png">
3. Previous <img title="Prev" alt="prev" height="24" width="24" src="https://github.com/StefanoDalMas/SpotifyDirectManager_MSP432/blob/main/src/images/spotify-controls/prev.png">
4. Volume Up <img title="Volume Up" alt="volume up" height="24" width="24" src="https://github.com/StefanoDalMas/SpotifyDirectManager_MSP432/blob/main/src/images/spotify-controls/volume-up.png">
5. Volume Down <img title="Volume Down" alt="volume down" height="24" width="24" src="https://github.com/StefanoDalMas/SpotifyDirectManager_MSP432/blob/main/src/images/spotify-controls/volume-down.png">

## Info
1. Display Song Name
2. Display Artist
3. Volume info by progress bar
4. Play/pause state by spinning logo

## Contributors
1. Stefano Dal Mas
* WiFi communication
* UART communication
* MSP interrupts
* HTTP requests
2. Ion Andy Ditu
* Spotify APIs
* JSON parsing
* HTTP requests
3. Amir Gheser
* Dataset refinement
* ML Model development

## Presentation and Demo

## References
1. [MSP432P401R Launchpad](https://www.ti.com/tool/MSP-EXP432P401R)
2. [MSP432P401R BoosterPack](https://www.ti.com/tool/BOOSTXL-EDUMKII)
3. [ESP32](https://www.espressif.com/en/products/socs/esp32)
4. [Spotify APIs](https://developer.spotify.com/documentation/web-api/)
5. [Arduino_JSON](https://arduinojson.org/)
6. [WiFi](https://www.arduino.cc/en/Reference/WiFi)
7. [HTTPClient](https://www.arduino.cc/en/Reference/HTTPClient)
8. [Driverlib](https://www.ti.com/tool/MSPDRIVERLIB)
9. [Grlib](https://www.ti.com/tool/MSP-GRLIB)
10. [Code Composer Studio](http://www.ti.com/tool/CCSTUDIO)
11. [Arduino IDE](https://www.arduino.cc/en/software)
12. [MSP432P401R Launchpad User's Guide](https://www.ti.com/lit/ug/slau356i/slau356i.pdf)
13. [MSP432P401R Launchpad Hardware User's Guide](https://www.ti.com/lit/ug/slau356j/slau356j.pdf)
14. [MSP432P401R Launchpad Software User's Guide](https://www.ti.com/lit/ug/slau356k/slau356k.pdf)
15. [MSP432P401R Launchpad BoosterPack User's Guide](https://www.ti.com/lit/ug/slau356l/slau356l.pdf)
16. [MSP432P401R Launchpad BoosterPack Hardware User's Guide](https://www.ti.com/lit/ug/slau356m/slau356m.pdf)
17. [MSP432P401R Launchpad BoosterPack Software User's Guide](https://www.ti.com/lit/ug/slau356n/slau356n.pdf)
18. [Picture to C code converter](https://www.digole.com/tools/PicturetoC_Hex_converter.php)
19. [Edge Impulse](https://studio.edgeimpulse.com/studio/176434)
20. [Dataset](http://download.tensorflow.org/data/speech_commands_v0.02.tar.gz](http://download.tensorflow.org/data/speech_commands_v0.02.tar.gz))

## Contact
1. [Stefano Dal Mas](https://github.com/StefanoDalMas) [stefano.dalmas@studenti.unitn.it]

2. [Andy Ion Ditu](https://github.com/Ion-Andy) [andyion.ditu@studenti.unitn.it]

3. [Amir Gheser](https://github.com/rogergheser) [amir.gheser@studenti.unitn.it]


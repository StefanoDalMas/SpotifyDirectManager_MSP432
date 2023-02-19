# SpotifyDirectManager_MSP432
Project for Embedded Software for the IoT

## Description
This project is a simple implementation of a Spotify Controller. Our project is based on the MSP432P401R Launchpad.
Our controller is able to play, pause, skip, and go back to the previous song. It is also able to change the volume of the song. The controller is able to do this by using the buttons on the Launchpad. The controller is also able to display the song name, artist, and album cover on the LCD screen. The controller is able to do this by using the Spotify APIs.
It exploits the Spotify API to get the song name, artist, and album cover. It then uses the UART protocol to send the data to the MSP432 which then displays the data on the LCD screen.
The ESP connects to the wifi using the wifi library and the http client library. It then uses the UART protocol to communicate with the MSP432. With the tokens we call the spotify APIs using the HTTP client library. We then parse the JSON data. We then send the data to the MSP using the UART protocol.

## Installation
1. Clone the repository
2. Open the project in Code Composer Studio
3. Connect the MSP432 to the computer
4. Build and run the project

## Usage
1. Connect the MSP432 to the computer
2. Build and run the project

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

## Requirements
### Hardware
1. MSP432P401R Launchpad
2. ESP8266

### Software
1. Spotify API
2. CCS
3. Driverlib
4. Grlib


## Credits
1. [Spotify API](https://developer.spotify.com/web-api/)
2. [MSP432P401R Launchpad](http://www.ti.com/tool/MSP-EXP432P401R)
3. [Energia](http://energia.nu/)
4. [ESP8266](https://www.espressif.com/en/products/hardware/esp8266ex/overview)

## Contact
1. [Stefano Dal Mas](stefano.dalmas@studenti.unitn.it)

2. [Andy Ion Ditu](andyion.ditu@studenti.unitn.it)

3. [Amir Gheser](amir.gheser@studenti.unitn.it)


// header file for voice recognition
// 
#ifndef VOICERECOGNITION_H
#define VOICERECOGNITION_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "source/ti/devices/msp432p4xx/inc/msp.h"
#include "source/ti/devices/msp432p4xx/driverlib/driverlib.h"
#include "source/ti/devices/msp432p4xx/driverlib/pcm.h"


// include the msgpdsp library for fast fourier transform
#define RECORDING_WINDOW_SIZE 256
#define RECORDING_FREQUENCY_NUM 256

enum {
    PLAY,
    PAUSE,
    STOP,
}

// define a struct training sample which holds a matrix spectograph and a label
typedef struct trainingSample {
    float spectograph[RECORDING_FREQUENCY_NUM][RECORDING_WINDOW_SIZE];
    int label;
} trainingSample;

void generateDataset(){
    // TODO()!
    //fix signature of the function, add paramenters and make sure all comments
    // describing what is to be done are fulfilled

    int spectrograph[RECORDING_FREQUENCY_NUM][RECORDING_WINDOW_SIZE];
    // for each frame of the recording window sample the input and apply the fft to obtain the frequency vector
    // for each recording frame assign a frequency vector thus generating the spectrograph.
    // The higher the frequency the higher the value in the frequency vector
    int frequencyVector[RECORDING_FREQUENCY_NUM];
    for (int i = 0; i < RECORDING_WINDOW_SIZE; i++)
    {
        // sample the input
        // apply the fft

        // assign the frequency vector to the spectrograph
        for (int j = 0; j < RECORDING_FREQUENCY_NUM; j++)
        {
            spectrograph[j][i] = frequencyVector[j];
        }
    }

    // This matrix represents an image where the higher the frequency the brighter the pixel
    // The rows represent the frequency bin and the columns represent the time frame
    // Later we can train by simply doing image recognition
    // store the spectrograph in a file
    // store the label in a file


}
void trainModel();
void testModel();
#endif

// Path: src/voiceRecognition.c
// Source file handling voice recognition
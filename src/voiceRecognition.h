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
#define RECORDING_WINDOW_SIZE 20
#define RECORDING_FREQUENCY_NUM 32

// typedef spectrograph_t as an 2d matrix of floats
typedef float spectrograph_t[RECORDING_FREQUENCY_NUM][RECORDING_WINDOW_SIZE];

enum {
    PLAY,
    PAUSE,
    STOP,
};

// define a struct training sample which holds a matrix spectograph and a label
typedef struct trainingSample {
    float spectograph[RECORDING_FREQUENCY_NUM][RECORDING_WINDOW_SIZE];
    int label;
} trainingSample;

typedef struct sample {
    float spectograph[RECORDING_FREQUENCY_NUM][RECORDING_WINDOW_SIZE];
    int label;
} sample;

typedef struct testSample {
    float spectograph[RECORDING_FREQUENCY_NUM][RECORDING_WINDOW_SIZE];
    int label;
} testSample;

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
void trainModel(trainingSample* trainingSamples, int numSamples){
    //
    // This function works if we use some libraries.
    // If we use something like yolo we can simply give the dataset 
    // to the model and train it like that
    // ==================

    // TODO()!
    // load the spectrograph and the label from the file
    // train the model using the spectrograph and the label
    // store the model in a file
    
    // we split the training data into training and validation data
    // we train the model using the training data and validate the model using the validation data
    // we can use the validation data to tune the hyperparameters of the model

    shuffle(trainingSamples, numSamples);
    int numTrainingSamples = numSamples * 0.8;
    int numValidationSamples = numSamples - numTrainingSamples;

    for (int i=0; i<numTrainingSamples; i++){
        // train the model using the training samples
    }

    for (int j=numTrainingSamples; j<numSamples; j++){
        // validate the model using the validation samples
    }
}
void testModel(trainingSample* trainingSamples, int numSamples){
    // TODO()!
    // load the spectrograph and the label from the file
    // load the model from the file
    // test the model using the spectrograph and the label
    // print the accuracy of the model
}

// write a function that shuffles an array of training samples
void shuffle(trainingSample* trainingSamples, int size){
    // TODO()!
    // shuffle the array of training samples
    for(int j=0; j<10; ++j){
        for(int i=0; i<size; i++){
            int randomIndex = rand() % size;
            swap(trainingSamples[i], trainingSamples[randomIndex]);
        }
    }
}

void swap(trainingSample &a, trainingSample &b){
    // TODO()!
    // swap the two training samples
    trainingSample temp = a;
    a = b;
    b = temp;
}

void exportModel(){
    // TODO()!
    // Decide how to export the model
}

#endif
// Path: src/voiceRecognition.c
// Source file handling voice recognition
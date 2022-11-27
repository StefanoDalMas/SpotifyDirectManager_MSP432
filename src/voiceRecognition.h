// header file for voice recognition
// 
// For info about the microphone specifics, see:
// BoosterPack Sensor Guide page 14/25
#ifndef VOICERECOGNITION_H
#define VOICERECOGNITION_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
// #include "source/ti/devices/msp432p4xx/inc/msp.h"
// #include "source/ti/devices/msp432p4xx/driverlib/driverlib.h"
// #include "source/ti/devices/msp432p4xx/driverlib/pcm.h"

#define minFreq 20
#define maxFreq 20000

// include the msgpdsp library for fast fourier transform
#define RECORDING_WINDOW_SIZE 2
#define RECORDING_FREQUENCY_NUM 256

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

void generateDataset();
void trainModel();
void testModel(trainingSample* trainingSamples, int numSamples);
void shuffle(trainingSample* samples, int numSamples);
void myswap(trainingSample *a, trainingSample *b);
void importModel(int** model);


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
            myswap(&trainingSamples[i], &trainingSamples[randomIndex]);
        }
    }
}

void myswap(trainingSample *a, trainingSample *b){
    // TODO()!
    // swap the two training samples
    trainingSample* temp = a;
    a = b;
    b = temp;
}

/**
 * @brief This function reads the parameters from "/../model_weights.txt"
 * 
 * @param model pointer to the future array holding the weights of the linear separators
 */
void importModel(int** model){
    int fd = open("./../model_weights.txt", O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Error opening file\n");
        perror("Error: ");
        exit(1);
    }
    FILE* logfile = fopen("log.txt", "w+");
    if (logfile == NULL){
        fprintf(stderr, "Error opening log file\n");
    }
    fprintf(logfile, "\n\nCommencing importing procedure...\n");
    fprintf(logfile, "Opening model_weights.txt...\n");
    FILE* file = fdopen(fd, "r");
    if (file == NULL) {
        fprintf(logfile, "Error opening file\n");
        perror("Error: ");
        exit(1);
    }
    fprintf(logfile, "Successfully opened model_weights.txt!\n");
    // load the model from the file
    // store the model in a variable
    
    char tmp = 0;
    tmp = getc(file);
    int classesNum = tmp - '0';
    printf("%d\n", classesNum);
    int featuresNum = 7;
    model = (int**)malloc(sizeof(int*) * tmp);
    for(int k=0; k<tmp; k++){
        model[k] = (int*)malloc(sizeof(int)*featuresNum);
    }
    int i=-1, j=0;
    printf("tmp: %c\n", tmp);
    tmp = getc(file); // to go to the next line
    printf("tmp: %c\n", tmp);
    while(!feof(file)){
        // read the model from the file
        if (tmp == '\n') {
            fprintf(logfile, "Read line no.%d\n", i);
            i++;
            j = 0;
        }
        else if (tmp == ',') {
            j++;
        } else if (tmp == ' '){
            fprintf(logfile, "Unexpected value\nAborting now.");
            fclose(logfile); fclose(file);
            exit(2);
        }
        else {
            printf("%c\n", tmp);
            model[i][j] = tmp - '0';
        }        
        printf("%c\n", tmp);
        tmp = getc(file);
    }
    fprintf(logfile, "Successfully read all %d lines.\nModel imported!\n", classesNum);
    printf("Successfully read all %d lines.\nModel imported!\n", classesNum);
    // testing the read of the model
    fprintf(logfile, "MODEL:\n");
    for(int i=0; i<classesNum; ++i){
        for (int j=0; j<featuresNum-1; ++j){
            fprintf(logfile, "%d,", model[i][j]);
        }
        fprintf(logfile, "%d\n", model[i][featuresNum-1]);
    }
    close(fd);
    fclose(file);
    fclose(logfile);
}

#endif
// Path: src/voiceRecognition.c
// Source file handling voice recognition
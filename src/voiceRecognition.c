// Source file handling voice recognition

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "voiceRecognition.h"

int main(int argc, char const *argv[])
{
    // Generate dataset for training
    generateDataset();
    // after generating the dataset trainingSamples we 
    // want to split it into a training set and a test set
    // traningSamples will be used to train the model and the latter to test it
    // Opt1: we store the dataset in a file and read it
    // Opt2: we make generateDataset() return a pointer to the dataset
    // Opt3: we make generateDataset() take a pointer to the dataset as a parameter

    // after loading the dataset into samples
    sample* samples; // samples should point to the data
    int numSamples = 0; // numSamples should be the number of samples in the dataset

    shuffle(samples, numSamples);
    int numTrainingSamples = numSamples * 0.8;

    // We're passing a pointer to the training samples and the number of training samples
    trainModel(samples, numTrainingSamples);
    // We're passing a pointer to the test samples (the offset is exactly the number of
    // training samples, hence the + numTrainingSamples)
    // and the number of test samples
    testModel(samples + numTrainingSamples, numSamples - numTrainingSamples);
    
    // Export the model so that the MSP can use it
    exportModel();
    /* code */
    return 0;
}
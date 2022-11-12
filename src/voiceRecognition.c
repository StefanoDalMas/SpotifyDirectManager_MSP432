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
    trainModel();
    testModel();
    /* code */
    return 0;
}
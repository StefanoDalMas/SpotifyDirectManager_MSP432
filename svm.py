import os
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import numpy as np
from sklearn import datasets, svm
from sklearn.model_selection import train_test_split
from sklearn import metrics
import deeplake
from scipy.fftpack import fft, ifft
from scipy.io import wavfile as wav
import time
import sys
import torch.utils.data.dataset
import torchaudio
import shutil
import subprocess
################
# Extra info on SVMs:
# https://scikit-learn.org/stable/modules/svm.html#
################
# NOTES
# Store values in a numpy dense array with dtype=np.float64
# If possible set the Kernel cache size to 500MB or 1000MB
# Setting C: the noisier the data the smaller C should be (default=1.0)
# Do some scaling to obtain good results
# Need to normalize the data !!!
################

commands = ['up', 'down', 'left', 'right', 'on', 'off', 'stop', 'one',
            'two', 'three', 'four', 'five', 'six', 'seven', 'eight', 'nine', 'zero']

options = ['test', 'train', 'valid']

def loadDataset1():
    # load the speech commands dataset using torchaudio
    # https://pytorch.org/audio/stable/datasets.html#speechcommands
    # https://pytorch.org/audio/stable/datasets.html#torchaudio.datasets.SPEECHCOMMANDS

    data = torchaudio.datasets.SPEECHCOMMANDS('.', download=True)
    testing_list_file = 'SpeechCommands/speech_commands_v0.02/testing_list.txt'
    validation_list_file = 'SpeechCommands/speech_commands_v0.02/validation_list.txt'
    
    ## Create directories if neeeded
    if not os.path.isdir("dataSet"):
        os.mkdir("dataSet")
        print("Created directory: dataSet")
    for option in options:
        if not os.path.isdir('dataSet/' + option):
            os.mkdir('dataSet/' + option)
        print("\tCreated directory: dataSet/" + option)
        for command in commands:
            if not os.path.isdir('dataSet/' + option + '/' + command):
                os.mkdir('dataSet/'+ option + '/' + command)
                print("\t\tCreated directory: dataSet/" + option + '/' + command)
    
    ## Populate directories
    os.chdir('dataSet')
    ret_val, output = subprocess.getstatusoutput('find . -name "*.wav" | wc -l')
    os.chdir('..')
    print("The folder contains " + "".join(output.split()) + " samples")
    if (int("".join(output.split())) == 0):
        print("Populating directories...")
        # TODO  populate directories
        # open the testing_list.txt and validation_list.txt files
        # read the paths and copy them in the correct directory using the shutil library

        fd_test = open(testing_list_file, "r")
        fd_valid = open(validation_list_file, "r")
        for line in fd_test:
            file_name = "".join(line.strip())
            tmp = line.split("/")
            if tmp[0] in commands:
                shutil.move('SpeechCommands/speech_commands_v0.02/' + file_name, 'dataSet/test/' + file_name)
        for line in fd_valid:
            file_name = "".join(line.strip())
            tmp = line.split("/")
            if tmp[0] in commands:
                shutil.move('SpeechCommands/speech_commands_v0.02/' + file_name, 'dataSet/valid/' + file_name)
        # Copy all the remaining files in the train directory
        # starting with name where name is in commands
        for command in commands:
            for file in os.listdir(f'SpeechCommands/speech_commands_v0.02/{command}/'):
                shutil.move(f'SpeechCommands/speech_commands_v0.02/{command}/{file}', f'dataSet/train/{command}/{file}')    
    else:
        print("Directory ready for usage")

    # credits to: https://arxiv.org/abs/1804.03209 - Warden 2018 dataset


    #################################################################################
    # TODO: convert wav files to fft and store them in a numpy array for later training
    #### ALMOST WORKING, NEED TO SPLIT THE WAV INTO LEFT AND RIGHT CHANNELS
    #### AND THEN CONVERT ONE OF THEM TO FFT
    rate, data = wav.read('dataSet/train/one/0a7c2a8d_nohash_0.wav')
    fft_out = np.fft.fft(data, 256)
    plt.plot(np.abs(fft_out), color='steelblue')
    plt.show()
    exit(1)


class Main:
    def __init__(self, X, y, clf: svm.SVC, num_labels, num_data):
        self.clf = clf
        self.num_labels = num_labels
        self.num_data = num_data
        self.X = X
        self.y = y
        clf.C = 1.0 # regularization parameter
        clf.kernel = 'rbf' # kernel type, rbf working fine here
        clf.cache_size = 200 # in MB - Note that it is advisable to set this value to 
                            # 500MB or 1000MB to avoid performance issues
        
        clf.decision_function_shape = "ovo" # "ovr" # One vs One / One vs Rest
        # Note that the LinearSVC also implements an alternative multi-class strategy, 
        # the so-called multi-class SVM formulated by Crammer and Singer [16], 
        # by using the option multi_class='crammer_singer'. In practice, 
        # one-vs-rest classification is usually preferred, since the results
        #  are mostly similar, but the runtime is significantly less.
        clf.fit(X_train, y_train)

    def test_model():
        # Test the model
        y_pred = clf.predict(X_test)
        print(y_pred)
        # Model Accuracy: how often is the classifier correct?
        print("Accuracy:",metrics.accuracy_score(y_test, y_pred))

    #funzione fatta da copilot spero funzioni
    def plot_confusion_matrix():
        # Plot non-normalized confusion matrix
        titles_options = [("Confusion matrix, without normalization", None),
                        ("Normalized confusion matrix", 'true')]
        for title, normalize in titles_options:
            disp = metrics.plot_confusion_matrix(clf, X_test, y_test,
                                                display_labels=num_labels,
                                                cmap='Blues',
                                                normalize=normalize)
            disp.ax_.set_title(title)

            print(title)
            print(disp.confusion_matrix)
        plt.show()

    def export_model():
        if clf.fit_status_ == 0:
            print("Model fit status: Successful")

            fd = open("model_weights.txt", "w")
            fd.write(str(clf.classes_.size)) # Write number of classes
            fd.close()
            np.savetxt("model_weights.txt", clf.coef_, delimiter=",", fmt="%s", newline="\n")
            # Weights should be enough.
            # In the C script we only need to do a dot product between the weights and the input 
            # and check whether the result is positive or negative.
            # We do this for each line and we return the one yielding a positve result.
            # If none of them does, we return the None.

def loadDataset():
        X = np.ndarray(shape=(num_data, 2, 512))
        y = np.ndarray((num_data, 1))
        # take samples from directory /../samples/ and 
        # store them in X and assign the directory name as label in y
        # open each file in the /../samples/ directory and store the data in X[i]
        #  and the name of the directory in y[i]
        j = 0
        for i in range(num_labels):
            for path in os.listdir(f"./samples/{i}"):
                print(path)
                #fd = open(f"./samples/{i}/{path}", "r")
                X = np.loadtxt(f"./samples/{i}/{path}", dtype=np.float64, delimiter=",")
                y[j] = i
                j += 1
                #fd.close()
        for i in range(num_data):
            print(f"{X[i]}\t label: {y[i]}")
        return X, y

# Testing
if __name__ == "__main__":
    # Imports
    # Generate own dataset
    # scales it as suggested in the notes to have mean 0 and std 1
    # X, y = testDataset() ## to use a sample dataset
    # change num_data to the actual amount of training data
    num_labels = 2
    num_data = 3 # change this to the actual amount of training data

    clf = svm.SVC()
    #===========================================================================
    train, test = loadDataset1()
    
    X_train, y_train = train
    X_test, y_test = test
    #X, y = loadDataset() ### TODO FINISH CLEANING UP THIS FUNCTION and MAIN
    #sys.exit(0)
    # Split into training and test data
    # X_train, X_test, y_train, y_test = train_test_split(
    #     X, y, test_size=0.2, random_state=123
    # )
    # Init the SVM model and train it
    Main.__init__(X_train, y_train, clf, num_labels, num_data)

    # Test the model
    Main.test_model(X_test, y_test)
    

# write an fast fourier transform function
# write a function to calculate the power spectrum of a signal
def fastFourierTransform(signal, num_samples):
    
    # calculate the power spectrum of a signal
    # signal is a 1D array of the signal
    # num_samples is the number of samples in the signal
    # returns the power spectrum of the signal

    # calculate the fourier transform of the signal
    fourier_transform = np.fft.fft(signal)

    # calculate the power spectrum
    power_spectrum = np.abs(fourier_transform)**2

    # calculate the frequencies
    frequencies = np.fft.fftfreq(num_samples)

    # return the power spectrum and the frequencies
    return power_spectrum, frequencies

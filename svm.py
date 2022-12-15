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
from pydub import AudioSegment
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
################
# Make sure commands and classes are the same and in the same order
commands = ['up', 'down', 'left', 'right', 'on', 'off', 'stop', 'one',
            'two', 'three', 'four', 'five', 'six', 'seven', 'eight', 'nine', 'zero']
classes = {'up': 0, 'down': 1, 'left': 2, 'right': 3, 'on': 4, 'off': 5, 'stop': 6, 'one': 7,
            'two': 8, 'three': 9, 'four': 10, 'five': 11, 'six': 12, 'seven': 13, 'eight': 14, 'nine': 15, 'zero': 16}
################
options = ['test', 'train', 'valid']

def createDirs():
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
    

def populateDirs(testing_list_file, validation_list_file):
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

def extractFFT(numSamples, bins, option):
    if option == 'train':
        print("Extracting FFTs for training set...")
    elif option == 'test':
        print("Extracting FFTs for testing set...")
    ## Extract FFT values
    #################################################################################
    # I'm not sure if this is the right way to do it, but for the time being this remains so.
    ## I don't think this is working yet...
    # allocate an array of size numSamples x (bins + command)
    # then proceed with filling the array with the fft values followed by the command
    # then reshape the array to the correct size
    # do not use append as it is very slow
    # only extract from train and test as we want to split these into train and test
    
    # create an matrix of size numSamples x bins + 1
    dt = np.dtype([('fft', np.float64, bins), ('command', np.str_, 10)])
    dataset = np.zeros((numSamples, bins + 1), dtype=dt)
    it = 0
    for command in commands:
        print(f'Extracting FFTs for {command}')
        for audio in os.listdir(f'dataSet/{option}/{command}'):
            # read the audio file
            tmp = AudioSegment.from_wav(f'dataSet/{option}/{command}/{audio}')
            data = np.array(tmp.get_array_of_samples())
            # extract the fft values
            fft_out = np.fft.fft(data, bins)
            # append the fft values to the array
            # TODO
            dataset[it]['fft'] = abs(fft_out)
            dataset[it]['command'] = command
            it += 1
            # plot fft results in a histogram
            # plt.hist(abs(fft_out), bins=bins)
            # plt.show()
    # reshape the array to the correct size
    dataset = dataset.reshape(numSamples, bins + 1)
    print(np.shape(dataset))

    return dataset


def loadDataset():
    # load the speech commands dataset using torchaudio
    # https://pytorch.org/audio/stable/datasets.html#speechcommands
    # https://pytorch.org/audio/stable/datasets.html#torchaudio.datasets.SPEECHCOMMANDS

    data = torchaudio.datasets.SPEECHCOMMANDS('.', download=True)
    testing_list_file = 'SpeechCommands/speech_commands_v0.02/testing_list.txt'
    validation_list_file = 'SpeechCommands/speech_commands_v0.02/validation_list.txt'
    
    createDirs()
    populateDirs(testing_list_file, validation_list_file)
    ## After splitting the original dataset into train, test and validation
    # we want to extract the fft values from the audio files    
    train_data = extractFFT(65589, 256, 'train')
    test_data = extractFFT(8192, 256, 'test')
    return train_data, test_data
    
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
    train, test = loadDataset()
    ############################
    # Normalise data
    # We calculate the mean for each variable and the standard deviation

    X_train = np.asmatrix(list(map(lambda x: x[0], train[:]["fft"])))
    y_train = np.asmatrix(list(map(lambda x: x[0], train[:]["command"])))
    X_test = np.asmatrix(list(map(lambda x: x[0], test[:]["fft"])))
    y_test = np.asmatrix(list(map(lambda x: x[0], test[:]["command"])))
    # WORKS SO FAR :D
    #===========================================================================
    invertedMat = X_train.T
    invertedMat1 = X_test.T
    means = np.zeros(len(invertedMat))
    stds = np.zeros(len(invertedMat))
    means1 = np.zeros(len(invertedMat1))
    stds1 = np.zeros(len(invertedMat1))
    for i in range(invertedMat):
        means[i] = np.mean(invertedMat[i])
        stds[i] = np.std(invertedMat[i])
        means1[i] = np.mean(invertedMat1[i])
        stds1[i] = np.std(invertedMat1[i])
    for i in range(X_train):
        for j in range(X_train[i]):
            X_train[i][j] = (X_train[i][j] - means[j]) / stds[j]

    for i in range(X_test):
        for j in range(X_test[i]):
            X_test[i][j] = (X_test[i][j] - means1[j]) / stds1[j]
    print("\n\nPrinting train data")
    print(X_train)
    print("\n\nPrinting test data")
    print(X_test)
    #===========================================================================
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

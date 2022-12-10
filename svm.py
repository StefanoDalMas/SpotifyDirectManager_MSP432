import os
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import numpy as np
from sklearn import datasets, svm
from sklearn.model_selection import train_test_split
from sklearn import metrics
import deeplake
from scipy.fftpack import fft
from scipy.io import wavfile
import time
import sys
import torch.utils.data.dataset
import torchaudio

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

def loadDataset1():
    # load the speech commands dataset using torchaudio
    # https://pytorch.org/audio/stable/datasets.html#speechcommands
    # https://pytorch.org/audio/stable/datasets.html#torchaudio.datasets.SPEECHCOMMANDS

    data = torchaudio.datasets.SPEECHCOMMANDS('.', download=True)
    # credits to: https://arxiv.org/abs/1804.03209 - Warden 2018 dataset
    data_loader = torch.utils.data.Dataloader(data, batch_size=1, shuffle=True)
    for i, (waveform, sample_rate, labels) in enumerate(data_loader):
        print("Waveform: {}\nSample rate: {}\nLabels: {}".format(waveform, sample_rate, labels))
        plt.figure()
        plt.plot(waveform.t().numpy())
        plt.show()


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


def fromWavToFFT(wav):
    data = wav
    a = data.T[0] # this is a two channel soundtrack, I get the first track
    b=[(ele/2**8.)*2-1 for ele in a] # this is 8-bit track, b is now normalized on [-1,1)
    c = fft(b) # create a list of complex number
    d = len(c)/2  # you only need half of the fft list
    plt.plot(abs(c[:(d-1)]),'r')
    plt.show()

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
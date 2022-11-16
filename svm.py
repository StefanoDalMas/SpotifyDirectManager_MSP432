import numpy as np
import os
import sys
################
# Extra info on SVMs:
# https://scikit-learn.org/stable/modules/svm.html#
################
# NOTES
# Store values in a numpy dense array with dtype=np.float64
# If possible set the Kernel cache size to 500MB or 1000MB
# Setting C: the noisier the data the smaller C should be (default=1.0)
# Do some scaling to obtain good results
################

def testDataset():
    X, y = datasets.make_blobs(
        n_samples=50, n_features=2, centers=2, cluster_std=1.05, random_state=40
    )
    y = np.where(y == 0, -1, 1)
    return X, y

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
    from sklearn.model_selection import train_test_split
    from sklearn import datasets
    import matplotlib.pyplot as plt
    # Generate own dataset
    # scales it as suggested in the notes to have mean 0 and std 1
    num_labels = 2
    #X, y = testDataset() ## to use a sample dataset
    # change num_data to the actual amount of training data
    num_data = 3 # change this to the actual amount of training data
    X, y = loadDataset() ### TODO FINISH CLEANING UP THIS FUNCTION and MAIN
    #sys.exit(0)

    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.2, random_state=123
    )
    from sklearn import svm
    clf = svm.SVC()
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
    # CHIEDERE A DAVIDE CHE NE PENSA

    clf.fit(X_train, y_train)
    predictions = clf.predict(X_test)

    #Plot the data
    def visualize_svm():
        def get_hyperplane_value(x, w, b, offset):
            return (-w[0] * x + b + offset) / w[1]

        fig = plt.figure()
        ax = fig.add_subplot(1, 1, 1)
        plt.scatter(X[:, 0], X[:, 1], marker="o", c=y)

        x0_1 = np.amin(X[:, 0])
        x0_2 = np.amax(X[:, 0])

        x1_1 = get_hyperplane_value(x0_1, clf.w, clf.b, 0)
        x1_2 = get_hyperplane_value(x0_2, clf.w, clf.b, 0)

        x1_1_m = get_hyperplane_value(x0_1, clf.w, clf.b, -1)
        x1_2_m = get_hyperplane_value(x0_2, clf.w, clf.b, -1)

        x1_1_p = get_hyperplane_value(x0_1, clf.w, clf.b, 1)
        x1_2_p = get_hyperplane_value(x0_2, clf.w, clf.b, 1)

        ax.plot([x0_1, x0_2], [x1_1, x1_2], "y--")
        ax.plot([x0_1, x0_2], [x1_1_m, x1_2_m], "k")
        ax.plot([x0_1, x0_2], [x1_1_p, x1_2_p], "k")

        x1_min = np.amin(X[:, 1])
        x1_max = np.amax(X[:, 1])
        ax.set_ylim([x1_min - 3, x1_max + 3])

        plt.show()

    visualize_svm()

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









# class SVM:

#     def __init__(self, learning_rate=0.001, lambda_param=0.01, n_iters=1000):
#         self.lr = learning_rate
#         self.lambda_param = lambda_param
#         self.n_iters = n_iters
#         self.w = None
#         self.b = None

#     def fit(self, X, y):
#         n_samples, n_features = X.shape

#         y_ = np.where(y <= 0, -1, 1)

#         # init weights
#         self.w = np.zeros(n_features)
#         self.b = 0

#         for _ in range(self.n_iters):
#             for idx, x_i in enumerate(X):
#                 condition = y_[idx] * (np.dot(x_i, self.w) - self.b) >= 1
#                 if condition:
#                     self.w -= self.lr * (2 * self.lambda_param * self.w)
#                 else:
#                     self.w -= self.lr * (2 * self.lambda_param * self.w - np.dot(x_i, y_[idx]))
#                     self.b -= self.lr * y_[idx]


#     def predict(self, X):
#         approx = np.dot(X, self.w) - self.b
#         return np.sign(approx)


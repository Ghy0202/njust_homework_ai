import os
import numpy as np
from scipy.io import loadmat
from matplotlib import pyplot

class CIFAR10:
    def __init__(self, is_corrupted= False):
        """
        Class of CIFAR10 dataset
        """
        assert os.path.exists("cifar10.mat"), \
            "There is no cifar10 dataset available in the main directory!"+\
            " Please place the mnist.mat file in the run directory."
        data = loadmat("cifar10.mat")
        self.train_x = data["train_x"]
        self.train_y = data["train_y"]
        self.test_x = data["test_x"]
        self.test_y = data["test_y"]
        self.n_train_size = self.train_x.shape[0]
        self.n_test_size = self.test_x.shape[0]

    def load(self, is_flatten=False, is_cast=False, is_gray=False):
        """
        Gives the CIFAR10 dataset
        :param is_flatten: if set true the training and test data are in the shape of a single dimension array.
        :param is_cast: if set true the data is between 0 and 1 (np.float32) instead of 0 and 255 (np.uint8)
        :return: A tuple of training and test dataset.  ((train_x, train_y), (test_x, test_y))
        """
        def rgb2gray(rgb):
            return np.float32(np.expand_dims(np.dot(rgb[..., :3], [0.2989, 0.5870, 0.1140]), axis=-1))

        if is_gray:
            self.train_x = rgb2gray(self.train_x)
            self.test_x = rgb2gray(self.test_x)

        if is_flatten:
            self.train_x = np.ndarray.reshape(self.train_x, (self.train_x.shape[0],
                                                             self.train_x.shape[1]*
                                                                 self.train_x.shape[2]*
                                                                 self.train_x.shape[3]))
            self.test_x = np.ndarray.reshape(self.test_x, (self.test_x.shape[0],
                                                           self.test_x.shape[1] *
                                                             self.test_x.shape[2]*
                                                             self.test_x.shape[3]))
        if is_cast:
            self.train_x = np.float32(self.train_x/255)
            self.test_x = np.float32(self.test_x/255)
        return (self.train_x, self.train_y), (self.test_x, self.test_y)



    def label_str(self):
     return ['airplane', 'automobile', 'bird', 'cat', 'deer', 'dog', 'frog', 'horse', 'ship', 'truck']

if __name__=='__main__':
    data=CIFAR10()
    print(data.train_x.shape)#32 32 3
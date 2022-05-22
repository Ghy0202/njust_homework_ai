######################################################################
#       softmax实现图片分类
#
#
#
#
#########################################################################
import matplotlib.pyplot as plt
import numpy as np
import random

class Softmax_Classification():
    def __init__(self,x,y):
        self.x=x
        self.y=y
        self.weight=np.random.rand(x.shape[1],y.shape[1])# feature*num

    def softmax(self,z):
        return np.exp(z)/np.sum(np.exp(z),axis=1).reshape(-1,1)

    def acc_cal(self):
        pass

    def GD(self):
        pass

    def SGD(self):
        pass

    def plot(self):
        pass


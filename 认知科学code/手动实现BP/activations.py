#################################
#       激活函数
#       tanh,softmax
#################################
import numpy as np

def sigmoid(x):
    return 1/(1+np.exp(-x))

def sigmoid_gradient(x):
    """对于sigmoid函数求导"""
    return sigmoid(x)*(1-sigmoid(x))

def tanh(x):
    """tanh激活函数"""
    return np.tanh(x)

def tanh_gradient(x):
    """对于tanh求导"""
    return 1-tanh(x)**2

def softmax(x):
    """softmax激活函数"""
    # 将输出变成概率，归一化
    if x.ndim==1:
        x=x.reshape(1,-1)#求转置
    f1=lambda x:np.exp(x-np.max(x))
    f2=lambda x:x/np.sum(x)
    #TODO： 沿着行的维度应用上述的变化
    x=np.apply_along_axis(f1,axis=1,arr=x)
    x=np.apply_along_axis(f2,axis=1,arr=x)
    return x


def softmax_gradient(x,label):
    """softmax梯度:s-y"""
    return softmax(x)-label
















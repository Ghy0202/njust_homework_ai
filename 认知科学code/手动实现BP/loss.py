###########################################
#       多分类的损失函数
#       (1)交叉熵损失函数
#       其他损失函数
#       (2)mle损失函数
#
###########################################


import numpy as np
def loss_error(y,h):
    # 交叉熵损失
    """onehot编码，y真实值,h预测值"""
    if y.ndim==1 or h.ndim==1:
        y=y.reshape(1,-1)
        h=h.reshape(1,-1)

    return y*np.log(h)


# 交叉熵损失函数
def cross_entropy(y,h):
    """onehot编码，y真实值,h预测值"""
    if y.ndim==1 or h.ndim==1:
        y=y.reshape(1,-1)
        h=h.reshape(1,-1)

    return -np.sum(y*np.log(h+1e-5))/y.shape[0]

def mle(y,h):
    if y.ndim==1 or h.ndim==1:
        y=y.reshape(1,-1)
        h=h.reshape(1,-1)
    return np.sum(h-y)



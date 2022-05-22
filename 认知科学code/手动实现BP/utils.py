#########################################
#       常用函数调用模块
#       （1）数据处理模块
#           生成训练集、测试集
#
#########################################
import numpy as np
import pickle
np.random.seed(2021)
from cifar10 import CIFAR10

def readSrc(filepath):
    #file1 = 'D:/homework_for_ML/task4/data/data_batch_1'
    fp = open(filepath, 'rb')
    dict = pickle.load(fp, encoding='iso-8859-1')
    fp.close()
    return dict

class DataLoader(object):
    def __init__(self,batch_size=64,data_type='train',scale=False):
        data=CIFAR10()
        # 加载灰度数据
        data.load(is_gray=True,is_cast=True)
        # 区分训练数据和测试数据
        if data_type=='train':
            self.data_size = data.train_x.shape[0]
            data_x, data_y = data.train_x, data.train_y
        elif data_type=='test':
            self.data_size=data.test_x.shape[0]
            data_x,data_y=data.test_x,data.test_y

        # 修改数据格式，使之可以适应训练需求
        self.data_x=data_x.flatten().reshape(data_x.shape[0],32,32)
        self.data_y=data_y.flatten()

        self.data_type=data_type
        self.data_num=len(self.data_y)
        self.index=0
        self.batch_size=batch_size
        self.end=False
        self.scale=scale


    def __iter__(self):
        return self

    def __next__(self):
        if self.end:
            raise StopIteration
        else:
            start=self.index
            end=self.index+self.batch_size
            if end<=self.data_num:
                # 若后继还有batch_size个，继续取数
                self.index+=self.batch_size
                x,y=self.data_x[start:end,...],self.data_y[start:end,...]
                if self.scale:
                    # 归一化
                    x=x/255
                return x,y
            else:
                if self.data_type=='test':
                    # 测试需要取完整的数据
                    x, y = self.data_x[start:self.data_num, ...], self.data_y[start:self.data_num, ...]
                    self.end = True
                    if self.scale:
                        x = x / 255.
                    return x, y
                else:
                    raise StopIteration

    def restart(self):
        # 迭代的下标置零，从头开始取数，保证迭代不会停止
        self.index=0


def onehot(labels,classes=None):
    """one hot 的编码实现"""
    if labels.ndim==1:
        labels=labels.reshape(-1,1)

    num_data=labels.shape[0]#批次，所以这里是数据数量
    index_offset=np.arange(num_data)*classes
    labels_onehot=np.zeros(shape=(num_data,classes))
    labels_onehot.flat[index_offset+labels.ravel()]=1
    return labels_onehot


if __name__=='__main__':
    """
     61.2111  44.9847  48.023  ... 137.0222 130.4358 128.7672]
   [ 18.8024   0.      10.0762 ...  94.6907  89.9901  94.0328]
   [ 23.9545   8.8914  31.4071 ...  90.2782  90.876   80.2191]
    
    """

    # 测试模块是否书写正确
    gen = DataLoader()
    for epoch in range(10):
        print("epoch")
        for step, (x, y) in enumerate(gen):
            print("ceshi:")
            print(step, x.shape, y.shape)
            print(x)
            print(y)

            break
        gen.restart()
    #0 (200, 200, 32, 32) (200,)
    # (200)

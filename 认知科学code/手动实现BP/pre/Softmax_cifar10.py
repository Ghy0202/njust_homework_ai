######################################################
#       分组作业1、2
#       Logistic & Softmax
#
######################################################
import numpy as np
import random
from tqdm import tqdm
from cifar10 import CIFAR10

class SoftmaxRegression():
    def __init__(self,X,y,lr=0.01,epoch=50):
        self.X=X
        self.y=y
        self.lr=lr
        self.epoch=epoch
        #self.weights=np.random.rand(self.X.shape[1],self.y.shape[1])# 特征维度，类别数目
        self.weights=np.ones(shape=(self.X.shape[1],self.y.shape[1]))# 特征维度，类别数目


    def softmax(self,z):
        # axis=1表示在第二维度上求和，也就是对应特征列求和
        return (np.exp(z))/np.sum(np.exp(z),axis=1).reshape(-1,1)

    def Countgrad(self,X,y,h):
        loss=-np.sum(y*np.log(h+1e-5))
        #print(h-y)
        grad=np.matmul(X.T,h-y)
        return loss,grad

    def evaluate(self,xx,yy):
        # 评估正确率
        h=self.softmax(np.matmul(xx,self.weights))
        h=np.argmax(h,axis=1)
        right=np.argmax(yy,axis=1)
        return np.sum(h==right)*1.0/xx.shape[0]

    def train(self):
        loss_list=[]
        #print(self.X[0],self.y[0])
        for e in tqdm(range(self.epoch)):
            # 更新参数
            h=self.softmax(np.matmul(self.X,self.weights))
            loss,grad=self.Countgrad(self.X,self.y,h)
            self.weights-=self.lr*grad
            loss_list.append(loss)

        return loss_list

# 一些验证
def loadData(filename):
    if(filename=='iris'):
        X_train = np.loadtxt('./data/Iris/train/x.txt', ndmin=2, dtype=float)
        X_test = np.loadtxt('./data/Iris/test/x.txt', ndmin=2, dtype=float)
        y_train = np.loadtxt('./data/Iris/train/y.txt', ndmin=2, dtype=int)
        y_test = np.loadtxt('./data/Iris/test/y.txt', ndmin=2, dtype=int)
        #print(X_train)
    elif filename=='cifar_10':
        data = CIFAR10()
        # 加载灰度数据
        data.load(is_gray=True, is_flatten=True,is_cast=True)
        X_train=data.train_x
        X_test=data.test_x
        y_test=data.test_y
        y_train=data.train_y
        #(50000, 1024)
        #(10000, 1)
        #(120, 2) (120, 1)
        # print(X_train.shape)
        # print(y_test.shape)
        # print(y_test)
    else:
        X_train=np.loadtxt('D:/homework_for_ML/task4/data/pca_train150_x.txt',ndmin=2,dtype=float)
        X_test=np.loadtxt('D:/homework_for_ML/task4/data/pca_test150_x.txt',ndmin=2,dtype=float)
        y_train=np.loadtxt('D:/homework_for_ML/task4/data/pca_train150_y.txt',ndmin=1,dtype=int)
        y_test=np.loadtxt('D:/homework_for_ML/task4/data/pca_test150_y.txt',ndmin=1,dtype=int)

    return X_train,y_train,X_test,y_test

def dataProcess(X,y):
    # train_x
    _X=np.ones((X.shape[0],X.shape[1]+1))# 加上偏置项
    _X[:,1:]=normolization(X)
    # train_y
    _y=onehot(y)
    # print(_y)
    return _X,_y

def onehot(y):
    _y=np.zeros((y.shape[0],np.max(y)+1))
    for i in range(y.shape[0]):
        _y[i,y[i]]=1
    return _y
    pass

def normolization(x):
    mean=np.mean(x,axis=0)
    avg=np.std(x,axis=0)
    return (x-mean)/avg
    #return x/255.0
    pass

def Exam_Softmax(filename,lr,epoch):
    X_train,y_train,X_test,y_test=loadData(filename)

    X_train,y_train=dataProcess(X_train,y_train)
    X_test,y_test=dataProcess(X_test,y_test)
    model=SoftmaxRegression(X_train,y_train,lr=lr,epoch=epoch)
    loss=model.train()
    print("train_acc:{:.4f}".format(model.evaluate(X_train,y_train)))
    print("test_acc:{:.4f}".format(model.evaluate(X_test,y_test)))
    import matplotlib.pyplot as plt
    plt.plot(loss,c='r')
    plt.show()



if __name__=='__main__':
    Exam_Softmax('cifar10',0.001,200)



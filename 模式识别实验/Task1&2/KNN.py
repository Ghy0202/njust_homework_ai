"""
最近邻算法的实现
"""
import numpy as np
import matplotlib.pyplot as plt
import math
from PCA.PCA import processPCA,PCA
from sklearn.model_selection import train_test_split
from Fisher.Fisher import Fisher
# 展示实验数据
def show_img_before():
    features = np.loadtxt("./data/ar_features.txt", dtype=float, delimiter=',')
    print(features.shape)
    for i in range(features.shape[0]):
        img = features[i].reshape((45, 50))
        plt.imshow(img)
        plt.show()
# 距离
def euclidean_distance(x1, x2):
    """ 欧式距离 """
    dist=np.sqrt(np.sum(np.square(x1-x2)))
    return dist

class KNN():
    def __init__(self,k=50):
        # 有120个人
        self.k=k

    def _vote(self,neighbor_id):
        # 选择最接近的邻居
        counts=np.bincount(neighbor_id.astype('int'))
        return counts.argmax()

    def process(self,x_train,y_train,x_test):
        # 训练
        y_pred=np.empty(x_test.shape[0])
        for i,test_sample in enumerate(x_test):
            idx=np.argsort([euclidean_distance(test_sample[0],x[0]) for x in x_train])[:self.k]
            k_nearest_neibours=np.array([y_train[i] for i in idx])
            y_pred[i]=self._vote(k_nearest_neibours)

        return y_pred
        # 预测

def ProcessKNN(k,x_train,y_train,x_test,y_test):
    # 测试KNN的正确率
    knn=KNN(k)
    y_pred=knn.process(x_train,y_train,x_test)
    # 显示正确率
    acc_num=sum(y_test==y_pred)
    all_num=y_test.shape[0]
    #print("应该输出的结果：",y_test)
    #print("预测结果：",y_pred)
    print("总数：",all_num,"正确个数：",acc_num,"正确率：",acc_num/(1.0*all_num))

def exam2_1(feat_num,test_rate):
    # 获取数据
    print("-------PCA----------")
    X,y=processPCA(feat_num)
    # X = np.loadtxt("./data/ar_features.txt", dtype=float, delimiter=',')
    # y = np.loadtxt("./data/ar_label.txt", dtype=int, delimiter=',')
    print(X.shape,y.shape)
    # 切分数据集
    x_train,x_test,y_train,y_test=train_test_split(
        X,y,test_size=test_rate,shuffle=True,random_state=1
    )
    print("训练集：", x_train.shape[0], "测试集：", x_test.shape[0])
    print("-------KNN----------")
    print(x_train.shape,y_train.shape)
    ProcessKNN(k=20,x_train=x_train,y_train=y_train,x_test=x_test, y_test=y_test)

def exam2_2(feat_num,test_rate):
    print('-----Fisher--------')
    fisher=Fisher(feat_num)
    X = np.loadtxt("./data/ar_features.txt", dtype=float, delimiter=',')
    y = np.loadtxt("./data/ar_label.txt", dtype=int, delimiter=',')
    X,y=fisher.fit(X,y)# 降维之后的数据
    print(X.shape, y.shape)
    # 切分数据集
    x_train, x_test, y_train, y_test = train_test_split(
        X, y, test_size=test_rate, shuffle=True, random_state=1
    )
    print("训练集：", x_train.shape[0], "测试集：", x_test.shape[0])
    print("-------KNN----------")
    print(x_train.shape, y_train.shape)
    ProcessKNN(k=20, x_train=x_train, y_train=y_train, x_test=x_test, y_test=y_test)

def exam2_3(feat_num1,feat_num2,test_rate):
    print('----PCA--Fisher----')
    X, y = processPCA(feat_num1)
    print(X.shape, y.shape)
    fisher=Fisher(feat_num2)
    X,y=fisher.fit(X,y)
    # 切分数据集
    x_train, x_test, y_train, y_test = train_test_split(
        X, y, test_size=test_rate, shuffle=True, random_state=1
    )
    print("训练集：", x_train.shape[0], "测试集：", x_test.shape[0])
    print("-------KNN----------")
    print(x_train.shape, y_train.shape)
    ProcessKNN(k=20, x_train=x_train, y_train=y_train, x_test=x_test, y_test=y_test)

def exam2_4(feat_num1,feat_num2,test_rate):
    print('-----Fisher-PCA--------')
    fisher = Fisher(feat_num1)
    X = np.loadtxt("./data/ar_features.txt", dtype=float, delimiter=',')
    y = np.loadtxt("./data/ar_label.txt", dtype=int, delimiter=',')
    X, y = fisher.fit(X, y)  # 降维之后的数据
    print(X.shape, y.shape)

    X=PCA(X,feat_num2)
    # 切分数据集
    x_train, x_test, y_train, y_test = train_test_split(
        X, y, test_size=test_rate, shuffle=True, random_state=1
    )
    print("训练集：", x_train.shape[0], "测试集：", x_test.shape[0])
    print("-------KNN----------")
    print(x_train.shape, y_train.shape)
    ProcessKNN(k=20, x_train=x_train, y_train=y_train, x_test=x_test, y_test=y_test)
'''测试PCA'''
#exam2_1(feat_num=1500,test_rate=0.2)
'''测试Fisher'''
#exam2_2(feat_num=15,test_rate=0.2)
'''先PCA再Fisher'''
#exam2_3(feat_num1=1000,feat_num2=20,test_rate=0.2)

'''先Fisher再PCA'''
exam2_4(feat_num1=20,feat_num2=10,test_rate=0.2)




"""
Yale数据集，K均值聚类
实验要求：
(1)编程实现K均值聚类，展示再选择不同初始参数时聚类算法的精度
(2)使用不同距离度量（L1 距离/欧式距离/马氏距离）方式的聚类结果
(3)将实验过程结果等图示展出
"""
import numpy as np
import scipy.io as scio
import matplotlib.pyplot as plt
from sklearn.metrics.pairwise import cosine_similarity
import pandas as pd


"""数据预处理"""
def Process1():
    # 处理数据：将特征和标签分开保存
    data=scio.loadmat("D:/homework_for_ML/模式识别ghy/Task12/data/Yale_15_11_100_80.mat")
    data=data['DAT']# 数据类型是8000 11 15，15个人 每个人有11张图片,像素是8000
    # 把数据存入csv中，[x][label]
    print(data.shape)
    data=data.transpose()
    print(data.shape)
    # 占用了
    feature_list=[]
    label_list=[]
    # (15, 11, 8000)
    for i in range(data.shape[0]):
        for j in range(data.shape[1]):
            label_list.append(i)
            feature_list.append(data[i][j])

    feature=np.asarray(feature_list)
    label=np.asarray(label_list)
    np.savetxt("./data/features.txt", feature, fmt='%f', delimiter=',')
    np.savetxt("./data/label.txt", label, fmt='%d', delimiter=',')

"""距离计算"""
def CountDis_eu(a,b):
    # 首先试一下欧式距离
    dis=np.linalg.norm(a-b)
    return dis

def CountDis_L1(a,b):
    #L1距离
    return sum([abs(a[i]-b[i]) for i in range(len(a))])

def CountDis_ma(a,b,covI):
    # 马氏距离
    #print(a.shape[0])
    a=a.reshape((1,a.shape[0]))
    b = b.reshape((1, b.shape[0]))
    # print(a.shape)
    # print(np.cov(a,b).shape)
    dis=(a-b)@covI@(a-b).T
    return dis

class KMeans():
    def __init__(self,data,label,k=15,max_iter=100,threshold=0.01):
        self.k=k
        self.threshold=threshold
        self.max_iter=max_iter
        self.data=data
        self.label=label
        self.N=data.shape[1]# 数据的特征维度
        self.m=data.shape[0]
        self.losses=[]

    def fit(self,dis_func="eu"):
        # 选取数据集中的样本
        if dis_func=="ma":
            data_t=np.mat(self.data).transpose()
            cov=np.cov(data_t)
            covI=(np.mat(cov)).I
        fig,ax=plt.subplots()
        centers=[]
        for i in range(self.k):
            centers.append(self.data[i*11])
        centers=np.asarray(centers)
        centers=np.random.random((self.k,self.N))
        cnt=0
        losses=[]
        acces=[]
        while(True):
            ax.cla()# 清除
            cnt+=1
            Clusters={}
            pred_lables=[]
            # 初始化聚类中心
            for i in range(self.k):
                Clusters[str(i)]=[centers[i]]
            # 迭代过程
            for i in range(self.m):
                # 对于每一个样例，计算应该隶属于的类
                if dis_func=="eu":
                    idx=np.argmin([CountDis_eu(self.data[i],center) for center in centers])
                elif dis_func=="l1":
                    idx = np.argmin([CountDis_L1(self.data[i], center) for center in centers])
                elif dis_func=="ma":
                    idx = np.argmin([CountDis_ma(self.data[i], center,covI) for center in centers])
                else:
                    idx = np.argmin([CountDis_eu(self.data[i], center) for center in centers])
                pred_lables.append(idx)
                # 添加到对应的簇内
                Clusters[str(idx)].append(self.data[i])
            # 对于每一类，重新计算质心
            self.pred_labels = np.asarray(pred_lables)
            loss=0
            for i in range(self.k):
                cluster=Clusters[str(i)]
                cluster=np.asarray(cluster)
                new_center=np.mean(cluster[0])
                loss+=np.sum(centers[i]-new_center)
                centers[i]=new_center
            losses.append(loss)# 更新数据
            acc=self.evaluate()
            acces.append(acc)
            ax.plot(losses,c='#800000',label='loss')
            ax.plot(acces,c='#191970',label='acc')
            ax.legend()
            plt.title("Process_acc_loss")
            plt.pause(0.3)# 暂停
            # 结束的条件
            #if cnt>self.max_iter or loss<self.threshold:
            if cnt>self.max_iter:
                self.pred_labels=np.asarray(pred_lables)
                break

        self.losses=losses

    def show_loss(self):
        plt.plot(self.losses)
        plt.show()

    def evaluate(self):
        """聚类结果和目标结果比较"""
        acc=np.sum(self.label==self.pred_labels)/(1.0*self.m)
        print("样本总数",self.m)
        print("聚类正确个数：",np.sum(self.label==self.pred_labels))
        print("Kmeans正确率:",acc)
        print(self.label)
        print(self.pred_labels)
        return acc


def doKmeans():
    # 加载数据
    labels=np.loadtxt("./data/label.txt",dtype=int)
    features=np.loadtxt("./data/features.txt",dtype=float,delimiter=',')
    """显示图片部分"""
    # for i in range(features.shape[0]):
    #     print(features[i].shape)
    #     img=features[i].reshape(80,100)
    #     plt.imshow(img)
    #     plt.show()
    """K均值聚类的过程"""
    kmeans=KMeans(features,labels,max_iter=500,threshold=0)
    kmeans.fit("ma")
    kmeans.evaluate()





















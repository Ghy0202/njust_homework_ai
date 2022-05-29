import numpy as np
import matplotlib.pyplot as plt
# fisher多类情况的讲解：https://blog.csdn.net/huangguohui_123/article/details/106190580
# fisher理论讲解:https://developer.aliyun.com/article/840175

import numpy as np

class Fisher():
    def __init__(self,w):
        """
        :param w: 降维的维度
        """
        self.w=w

    def fit(self,X,y):
        # 形状(1680,xxx)
        # 计算组间方差矩阵B
        self.m={}# 存储均值的
        self.m0=X.mean(axis=0)
        kind=-1
        C=X[0]
        for i in range(y.shape[0]):
            if y[i]!=kind:
                if kind!=-1:
                    self.m[str(kind)]=np.mean(C,axis=0)
                kind=y[i]
                C=X[i]
                continue
            C=np.vstack((C,X[i]))
        self.m[str(kind)] = np.mean(C, axis=0)
        self.W=None
        for i in range(y.shape[0]):
            mat=np.matrix(X[i]-self.m[str(y[i])])
            S_wi=mat.T@mat
            if self.W is None:
                self.W=S_wi
            else:
                self.W+=S_wi
        # 计算组间方差矩阵B
        self.B=None
        for key,value in self.m.items():
            mat=np.matrix(value-self.m0)
            S_bi=mat.T@mat
            if self.B is None:
                self.B=S_bi
            else:
                self.B+=S_bi
        # 计算W.I·B
        print(self.W.I.shape,self.B.shape)
        self.WI_B=self.W.I@self.B
        # 将得到的特征值从大到小排列，取非零的特征值对应的特征向量
        fea_value,fea_vector=np.linalg.eig(self.WI_B)
        index = np.argsort(-fea_value)# 重新排序
        print("被选中的特征值的下标",index)
        # 这边需要选取非0的:资料上说是接近0的……自己指定叭
        select_vec=np.matrix(fea_vector.T[index[:self.w]])
        # 将样本点投射到选取的特征向量上
        print(select_vec.shape,X.shape)
        data=X@select_vec.T
        return data,y

# 尝试debug
# features=np.loadtxt("../data/ar_features.txt",dtype=float,delimiter=',')
# labels=np.loadtxt("../data/ar_label.txt",dtype=int,delimiter=',')
# fisher=Fisher(200)
# data,y=fisher.fit(features,labels)
# print("降维结果：",data.shape)









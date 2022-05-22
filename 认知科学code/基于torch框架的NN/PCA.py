# 数据的降维
from sklearn.datasets import fetch_lfw_people #导入人脸识别数据集库
from sklearn.decomposition import PCA         #导入降维算法PCA模块
import matplotlib.pyplot as plt
import numpy as np



x=np.random.randn(2,20)
print(x)

pca=PCA(2).fit(x)   #实例化 拟合
x_dr=pca.transform(x) #训练转换
print(x_dr)


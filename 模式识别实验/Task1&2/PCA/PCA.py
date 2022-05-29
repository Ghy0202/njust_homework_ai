# PCA降维算法的实现
import numpy as np
import matplotlib.pyplot as plt

"""归一化"""
def norm(X):
    """归一化(0,1)"""
    X_std = (X - X.min(axis=0)) / (X.max(axis=0) - X.min(axis=0))
    return X_std
"""PCA测试"""
def processPCA(feat_num=20):
    features=np.loadtxt("./data/ar_features.txt",dtype=float,delimiter=',')
    labels=np.loadtxt("./data/ar_label.txt",dtype=int,delimiter=',')
    # 将数据归一化
    features=norm(features)
    pca_features=PCA(features,feat_num)
    print(pca_features.shape)
    return pca_features,labels

"""PCA降维"""
def PCA(data,feat_num):
    """
    对AR数据集进行实验
    :param data: array格式，二维(数量id，特征)
    :return:降维结果
    """
    print("PCA之前数据维度：",data.shape)
    m,n=np.shape(data)
    # 取平均值，每一列
    avg=np.mean(data,axis=0)
    # 拓展均值
    avgs=np.tile(avg,(m,1))
    # 原始数据-均值
    data_sub=data-avgs
    # 求data的协方差矩阵的特征值和特征向量
    cov=np.cov(data_sub.T)
    fea_value,fea_vec=np.linalg.eig(cov)
    # 选择最大的K个特征值对应的k个特征向量
    index=np.argsort(-fea_value)
    # 因为特征向量是列向量，这里转化为横向量
    select_vec=np.matrix(fea_vec.T[index[:feat_num]])
    # 将样本点投影到选取的特征向量上
    pca_data=data_sub@select_vec.T

    """计算重构误差"""
    # 还原对应投影后的数据
    recon_data=(pca_data*select_vec)+avgs
    err_mat=data-recon_data
    err1=np.sum(np.array(err_mat)**2)/n
    # print("重构误差：",err1)
    # # 计算n
    # err2=np.sum(data_sub**2)/n
    # print(err2)
    # print("保留信息：",1-err1/err2)
    return pca_data









"""
1) 基本要求
将 AR 人脸数据集划分成训练集和测试集，实现主成份分析算法，
将人脸图像压缩成低维特 征向量，并结合最近邻分类算法计算分类精度，
分析实验结果。
2) 中级要求
展示用主成分分析将模式样本压缩成不同特征维度时，模式分类准确率。
3) 高级要求
对人脸数据集先进行主成分分析，在此基础上再做 Fisher 判别分析，计算分类精度，分析实 验结果。 4)拓展要求：深入探讨主成分分析和 Fisher 判别分析的关系，以及对当前主流表示学习方法 的启示。
"""
import numpy as np
import scipy.io as scio
import matplotlib.pyplot as plt
from sklearn.metrics.pairwise import cosine_similarity
import pandas as pd

def dataProcess():
    # 处理，查看数据
    data=scio.loadmat("D:/homework_for_ML/模式识别ghy/Task12/data/AR_120_14_50_45.mat")
    data=data['DAT']
    # (2250, 14, 120)
    print(data.shape)
    data=data.transpose()
    print(data.shape)
    feature_list=[]
    label_list=[]
    for i in range(data.shape[0]):
        for j in range(data.shape[1]):
            label_list.append(i)
            feature_list.append(data[i][j])

    feature=np.asarray(feature_list)
    label=np.asarray(label_list)
    np.savetxt("./data/ar_features.txt", feature, fmt='%f', delimiter=',')
    np.savetxt("./data/ar_label.txt", label, fmt='%d', delimiter=',')

# dataProcess()


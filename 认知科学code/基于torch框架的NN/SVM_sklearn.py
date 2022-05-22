###############################################
#           基于sklearn的svm
#           参数解释：
#           见：https://blog.csdn.net/u012915522/article/details/98853747
###############################################
from sklearn import svm,datasets
from sklearn.model_selection import train_test_split
import numpy as np


def testSVM():
    file=open('D:/homework_for_ML/task4/基于torch框架的NN/data/gray_src_.txt')
    data=[]
    labels=[]
    for line in file.readlines():
        curline=line.split()
        floatline=list(map(float,curline))# 用map函数将数据转化成float类型,py3中需要额外zhuanhuachenglist
        data.append(floatline[1:1025])
        labels.append(floatline[0])
    file.close()
    print(data)

    # 拆分测试集-训练集
    x_train,x_test,y_train,y_test=train_test_split(data,labels,random_state=14)


    # 定义模型
    predictor = svm.SVC(gamma='scale', C=1.0, decision_function_shape='ovr', kernel='rbf')
    # 进行训练
    predictor.fit(x_train, y_train)

    # 测试验证结果
    # 预测结果
    result = predictor.predict(x_test)
    # 进行评估
    from sklearn.metrics import f1_score
    print("F-score: {0:.2f}".format(f1_score(result, y_test, average='micro')))


testSVM()

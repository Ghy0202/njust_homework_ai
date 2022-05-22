############################################
#   测试LibSvm在图像分类上的表现
#
#---------------------------------------------------------
#   参数说明
#---------------------------------------------------------
#-s svm_type : set type of SVM (default 0)
#	0 -- C-SVC
#	1 -- nu-SVC
#	2 -- one-class SVM
#	3 -- epsilon-SVR
#	4 -- nu-SVR
#-t kernel_type : set type of kernel function (default 2)
#	0 -- linear: u'*v
#	1 -- polynomial: (gamma*u'*v + coef0)^degree
#	2 -- radial basis function: exp(-gamma*|u-v|^2)
#	3 -- sigmoid: tanh(gamma*u'*v + coef0)
#-h shrinking: whether to use the shrinking heuristics, 0 or 1 (default 1)
#是否使用收缩试探法
#
############################################
from libsvm.svm import *
from libsvm.svmutil import *
from libsvm.commonutil import *
import argparse


# 数据集加载
train_label,train_value=svm_read_problem('./data/gray_scale_train_svm.txt')  #改变了函数的存放位置:target只有一个
test_label,test_value=svm_read_problem('./data/gray_scale_test_svm.txt')
print('*'*50)
print("数据集加载完成")
# 模型加载
# 之后可以试着调整模型的参数看看情况
# svm_train(y[:200], x[:200], '-c 4' )


def Exam(t,c):
    model=svm_train(train_label,train_value,t+' '+c)
    print('*' * 50)
    print("训练完成")
    # 正确率的检验
    print('*' * 50)
    print("*"*50)
    print("test:")
    print(t,c)
    p_label, p_acc, p_val = svm_predict(train_label, train_value, model)
    print(p_acc)
    p_label, p_acc, p_val = svm_predict(test_label, test_value, model)
    print(p_acc)

if __name__=='__main__':
    # 首先是线性核函数
    #Exam('-t 0','-c 10')
    #Exam('-t 0','-c 1000000000')
    # polynominal核函数
    #Exam('-t 1', '-c 10')
    #Exam('-t 1', '-c 1000000000')#软间隔设置很小
    # radial核函数
    #Exam('-t 2', '-c 10')
    #Exam('-t 2', '-c 10')#test: 38.52%
    #Exam('-t 2', '-c 100')
    # sigmoid核函数
    #Exam('-t 3', '-c 10')
    #Exam('-t 3', '-c 10000000')
    # 首先是线性核函数
    parser=argparse.ArgumentParser()
    parser.add_argument("--t",default="2",help="0:linear, 1:polynominal, 2:radial, 3:sigmoid")
    parser.add_argument("--c",default="10",help="0->1000000:hard->soft")
    parser.add_argument("--save",default=False,help="save or not",type=bool)
    args=parser.parse_args()
    Exam("-t "+args.t,"-c "+args.c)















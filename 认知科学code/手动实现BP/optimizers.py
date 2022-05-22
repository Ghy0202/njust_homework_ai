########################################################################################
#       优化器：
#           一直都是直接调用库函数，从来没有注意过具体实现，感谢这次有机会实现：
#           深度学习流行优化方式公式算法见此专栏：https://zhuanlan.zhihu.com/p/90169812
#               SGD
#           *   Adam(目前最流行的方式)
#               Ada
#               RMSProp
#######################################################################################
import numpy as np
np.random.seed(2021)

class SGD(object):
    def __init__(self,lr=0.001):
        self.lr=lr
    def optimize(self,weight_num,params,grad,batch_size):
        """随机梯度下降，按照批次更新权重"""
        for i in range(1,weight_num+1):
            params['w'+str(i)]-=self.lr*grad['dw'+str(i)]/batch_size
        return params

class SGD_M(object):
    """
    带动量的SGD
    """
    def __init__(self,lr=0.001):
        pass

    def optimize(self,weight_num,params,grad,batch_size,bn=False):
        pass


class Adam(object):
    def __init__(self,lr=0.0001,beta1=0.9,beta2=0.999,epsilon=1e-8,weights=None,weight_num=None):
        """
        Adam优化器
        :param lr:
        :param beta1:
        :param beta2:
        :param epsilon:
        :param weights:
        :param weight_num:
        """
        self.lr=lr
        self.t=0
        self.beta1=beta1
        self.beta2=beta2
        self.epsilon=epsilon
        self.m=dict()
        self.v=dict()
        self.r=dict()
        for i in range(1,weight_num+1):
            self.m['m'+str(i)]=np.zeros(shape=weights['w'+str(i)].shape)
            self.v['v'+str(i)]=np.zeros(shape=weights['w'+str(i)].shape)

    def optimize(self,weight_num,params,grads,batch_size=64):
        """
        Adam优化的方式：
        :param weight_num:
        :param params:
        :param grads:
        :param batch_size:
        :return:
        """
        self.t+=1
        for i in range(1,weight_num+1):
            w=params['w'+str(i)]
            g=grads['dw'+str(i)]/batch_size
            self.m['m'+str(i)]=self.beta1*self.m['m'+str(i)]+(1-self.beta1)*g
            self.v['v'+str(i)]=self.beta2*self.v['v'+str(i)]+(1-self.beta2)*(g**2)
            m_hat=self.m['m'+str(i)]/(1-self.beta1**self.t)
            v_hat=self.v['v'+str(i)]/(1-self.beta2**self.t)
            w=w-self.lr*m_hat/(np.sqrt(v_hat)+self.epsilon)
            params['w'+str(i)]=w    #   更新第i层的权重
        return params

class AdaGrad(object):
    def __init__(self,lr=0.0001,epsilon=1e-7,weights=None,weight_num=None):
        self.epsilon=epsilon# 全局最小
        self.lr=lr
        self.r=dict()
        for i in range(1,weight_num+1):
            self.r['r'+str(i)]=np.zeros(shape=weights['w'+str(i)].shape)



    def optimize(self,weight_num,params,grads,batch_size=64):
        for i in range(1,weight_num+1):
            w=params['w'+str(i)]
            g=grads['dw'+str(i)]/batch_size
            # 积累平方梯度
            self.r['r'+str(i)]+=g*g
            dw=g*self.lr/(self.epsilon+np.sqrt(self.r['r'+str(i)]))
            params['w'+str(i)]-=dw
        return params

class RMSProp(object):
    def __init__(self,lr=0.0001,epsilon=1e-7,mu=0.9,weights=None,weight_num=None):
        self.epsilon=epsilon# 全局最小
        self.lr=lr
        self.mu=mu # 衰减速率
        self.r=dict()
        for i in range(1,weight_num+1):
            self.r['r'+str(i)]=np.zeros(shape=weights['w'+str(i)].shape)



    def optimize(self,weight_num,params,grads,batch_size=64):
        for i in range(1,weight_num+1):
            w=params['w'+str(i)]
            g=grads['dw'+str(i)]/batch_size
            # 积累平方梯度-----与AdaGrad唯一的区别
            self.r['r'+str(i)]=self.r['r'+str(i)]*self.mu+(1-self.mu)*g*g
            dw=g*self.lr/(self.epsilon+np.sqrt(self.r['r'+str(i)]))
            params['w'+str(i)]-=dw
        return params







############################################################################################
#       构建FNN反向梯度更新模型
#           需要手动构建dropout
#           需要自动的网络层数
#           需要自动的隐层结点个数
#           损失函数
#           激活函数
#       结论：
#############################################################################################
import numpy as np
from tqdm import tqdm
from time import sleep
# 自己实现的相关工具
from initializers import xavier,zeros
from optimizers import Adam,SGD,AdaGrad,RMSProp
from activations import softmax,tanh,softmax_gradient,tanh_gradient,sigmoid,sigmoid_gradient
from utils import onehot
from loss import cross_entropy,loss_error,mle




def dropout(x,dropout_prob):
    keep_prob=1-dropout_prob
    d_temp=np.random.binomial(1,keep_prob,size=x.shape[1:])/keep_prob
    #print(d_temp)
    d_temp=d_temp.reshape(-1)
    #print(d_temp)
    x_dropout=x*d_temp
    return x_dropout,d_temp



class BPNN(object):
    def __init__(self,num_layers,unit_list=None,initializer=None,optimizer='adam',activate='tanh',loss_fn='mle',dropout=0,batch_size=64):
        """

        :param num_layers: 层数
        :param unit_list: 隐层参数
        :param initializer: 初始化函数
        :param optimizer: 优化器
        :param activate: 激活函数
        """
        self.hidden_num=num_layers-1
        self.loss_fn=loss_fn
        self.batch_size=batch_size
        self.dropout=dropout
        # 初始化的方式
        if initializer=='xavier':
            self.params=xavier(num_layers,unit_list)
        else:
            self.params=zeros(num_layers,unit_list)
        # 激活函数
        self.activate=activate
        # 优化方式为随机梯度下降
        if optimizer=='sgd':
            print("sgd")
            self.optimizer=SGD()
        elif optimizer=='adam':
            self.optimizer=Adam(weights=self.params, weight_num=self.hidden_num)

        elif optimizer=='ada':
            print("ada")
            self.optimizer=AdaGrad(weights=self.params, weight_num=self.hidden_num)
        elif optimizer=='rmsprop':
            print("rmsprop")
            self.optimizer=RMSProp(weights=self.params, weight_num=self.hidden_num)
        else:
            # 默认是最好的优化器
            self.optimizer=Adam(weights=self.params, weight_num=self.hidden_num)



    def forward(self,x,dropout_prob=None):
        """
        前向传播，针对一个批次处理数据
        """
        net_inputs=[]#各层的输入
        net_outputs=[]#各层输出
        net_d=[]
        # 为了层号对应，将输入层直接添加
        net_inputs.append(x)
        net_outputs.append(x)
        net_d.append(np.ones(x.shape[1:]))# 输入层没有丢弃概率
        for i in range(1,self.hidden_num):
            x=x@self.params['w'+str(i)].T# @是矩阵的叉乘
            net_inputs.append(x)
            # 激活函数
            if self.activate=='tanh':
                x=tanh(x)
            else:
                x=sigmoid(x)
            if dropout_prob:
                # 丢弃一些参数
                x,d_temp=dropout(x,dropout_prob)
                net_d.append(d_temp)#某一层的遮罩
            net_outputs.append(x)
        out=x@self.params['w'+str(self.hidden_num)].T # 最后一层的输出
        net_inputs.append(out)# 本层的输出作为下一次的输入
        out=softmax(out)
        net_outputs.append(out)
        # 用字典储存每一层的状况，便于backpropagation
        return {'net_inputs':net_inputs,'net_outputs':net_outputs,'d':net_d},out

    def backward(self,nets,y,h,drop_prob=None):
        """反向更新参数"""
        # 最后一层单独讨论
        grads=dict()#反向存储梯度
        if self.loss_fn=='mle':
            grads['dz'+str(self.hidden_num)]=h-y    # 最后一层的梯度就是”预测值-真实值"
        else:
            grads['dz' + str(self.hidden_num)] = loss_error(h,y)
        # 这边，可以换成其他的损失函数
        grads['dw'+str(self.hidden_num)]=grads['dz'+str(self.hidden_num)].T@nets['net_outputs'][self.hidden_num-1]#误差*输入

        #其他层套用模板
        for i in reversed(range(1,self.hidden_num)):
            # 误差*该层的梯度
            if self.activate=='tanh':
                temp=grads['dz'+str(i+1)]@self.params['w'+str(i+1)]*tanh_gradient(nets['net_inputs'][i])
            else:
                temp=grads['dz'+str(i+1)]@self.params['w'+str(i+1)]*sigmoid_gradient(nets['net_inputs'][i])
            if drop_prob:
                temp=temp*nets['d'][i]/(1-drop_prob)
            grads['dz'+str(i)]=temp#当前层的结果，是需要输入前一层的误差
            grads['dw'+str(i)]=grads['dz'+str(i)].T@nets['net_outputs'][i-1]
        return grads


    def train(self,dataloader,valid_loader,epoch,lr,dropout_prob=None):
        """

        :param dataloader:
        :param valid_loader:
        :param epoch:
        :param lr:
        :param dropout_prob:
        :return:
        """
        train_loss=[]
        test_loss=[]
        epochs=epoch
        for epoch in range(epochs):
            epoch_loss = 0.0
            for step, (x, y) in enumerate(dataloader):
                x = x.reshape(-1, 32 * 32)
                y = onehot(y, 10)
                nets, h = self.forward(x, dropout_prob)
                if self.loss_fn == 'mle':
                    loss = cross_entropy(y, h)
                else:
                    loss=cross_entropy(y,h)
                epoch_loss += loss
                grads = self.backward(nets, y, h, dropout_prob)
                self.params = self.optimizer.optimize(self.hidden_num, self.params, grads, self.batch_size)
                if step % 100 == 0:
                    print("epoch:{} step:{} loss:{:.4f}".format(epoch,step,loss))
            train_loss.append(epoch_loss)
            print("*"*50)
            print("epoch_loss:",epoch_loss)
            dataloader.restart()
            # 验证部分一个
            epoch_loss_test = 0.0

            # for step, (x, y) in enumerate(valid_loader):
            #     x = x.reshape(-1, 32 * 32)
            #     y = onehot(y, 10)
            #     nets, h = self.forward(x, dropout_prob)
            #     loss = cross_entropy(y, h)
            #
            #     epoch_loss_test += loss
            # test_loss.append(epoch_loss_test)
            # valid_loader.restart()

        loss_all = {'train_loss': train_loss, 'valid_loss': test_loss}
        return loss_all

    def predict(self,data_loader,bn=False):
        labels=[]#真实的标签值
        h=[]#预测值
        losses=0
        for (x,y) in data_loader:
            x=x.reshape(-1,32*32)#将数据展开成为1维向量
            y=onehot(y,10)#标签，输出为10类
            if bn:
                # TODO:正则化层
                _,out=self.forward(x,'test')
            else:
                _,out=self.forward(x)
            loss=cross_entropy(y,out)
            losses+=loss
            out=list(np.argmax(out,axis=-1).flatten())
            y=list(np.argmax(y,axis=1).flatten())
            labels+=y
            h+=out

        return np.array(h).astype('int'), np.array(labels).astype('int')










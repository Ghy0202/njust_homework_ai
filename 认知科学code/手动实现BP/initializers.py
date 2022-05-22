##############################################
#       初始化的一些函数
#       (1)xavier
#       (2)zero
##############################################
import numpy as np
#固定随机化种子
np.random.seed(2021)

def xavier(num_layers,units_list):
    """
    保证参数服从[-a,a]的均匀分布，其中a为sqrt(6)/sqrt(n_in+n_out)
    :param num_layers:
    :param units_list:
    :return:
    """
    params={}#每一层的参数用字典
    for layer in range(1,num_layers):
        # 第一层是输入层，没有参数，之后每一层都有参数
        a=np.sqrt(6)/np.sqrt(units_list[layer-1]+units_list[layer])
        params['w'+str(layer)]=np.random.uniform(-a,a,size=(units_list[layer],units_list[layer-1]))
        params['gamma'+str(layer)]=np.ones(shape=(1,units_list[layer]))#偏置项
        params['beta'+str(layer)]=np.zeros(shape=(1,units_list[layer]))

    return params


def zeros(num_layers,units_list):
    """
    全零初始化
    :param num_layers:
    :param units_list:
    :return:
    """
    params = {}  # 每一层的参数用字典
    for layer in range(1, num_layers):
        params['w' + str(layer)] = np.zeros(shape=(units_list[layer], units_list[layer - 1]))
        params['gamma' + str(layer)] = np.ones(shape=(1, units_list[layer]))  # 偏置项
        params['beta' + str(layer)] = np.zeros(shape=(1, units_list[layer]))  # TODO：但是为什么要加这一层？

    return params








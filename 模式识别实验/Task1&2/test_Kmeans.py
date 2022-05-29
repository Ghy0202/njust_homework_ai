import numpy as np
from PCA.PCA import PCA
from Fisher.Fisher import Fisher
from Kmeans.Kmeans_Yale import KMeans

def test(feat_num1):
    labels = np.loadtxt("./Kmeans/data/label.txt", dtype=int)
    features = np.loadtxt("./Kmeans/data/features.txt", dtype=float, delimiter=',')
    X=PCA(features,feat_num=feat_num1)
    print(X.shape)
    """显示图片部分"""
    # for i in range(features.shape[0]):
    #     print(features[i].shape)
    #     img=features[i].reshape(80,100)
    #     plt.imshow(img)
    #     plt.show()
    """K均值聚类的过程"""
    kmeans = KMeans(X, labels, max_iter=500, threshold=0)
    kmeans.fit("ma")
    kmeans.evaluate()

if __name__=='__main__':

    test(20)
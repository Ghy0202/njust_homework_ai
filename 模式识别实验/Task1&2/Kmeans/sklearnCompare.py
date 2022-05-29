# 对比看实验精度
from sklearn.cluster import KMeans
import numpy as np
def compare():
    labels = np.loadtxt("./data/label.txt", dtype=int)
    features = np.loadtxt("./data/features.txt", dtype=float, delimiter=',')
    k_means = KMeans(n_clusters=15, random_state=10)
    k_means.fit(features)
    pred=k_means.predict(features)
    print("sklearn ACC:",np.sum(pred==labels))

compare()
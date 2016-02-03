import numpy as np

def rankdiag(q,xtrue,xest):
    v = np.percentile(xtrue,q)
    vret = np.zeros(len(q)-1)
    for i in range(0,len(q)-1):
        vret[i]=((v[i] <= xest) & (xest < v[i+1])).sum()
    vret[0]+=(xest < v[0]).sum()
    vret[-1]+=(v[-1] <= xest).sum()
    return(vret)

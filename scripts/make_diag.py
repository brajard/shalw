import os
import sys
sys.path.reverse() #look in anaconda paths first
from netCDF4 import Dataset
import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import chisquare



def compute_cost(dirname,suff='1',bfile = 'state_bck_',afile='state_ret_',ofile='obs_per_',scoef='scoef.i',bcoef='bcoef.i'):
    obfile = ofile + suff + '.dat'
    obs = load_obs(dirname,obsfile)
    fa = Dataset(os.path.join(dirname,afile + suff + '.nc'))
    xa = fa.variables['Hfil'][:]
    fa.close()
    fb = Dataset(os.path.join(dirname,bfile + suff + '.nc'))
    xb = fb.variables['Hfil'][:]
    fb.close()
    binv = read_pert(dirname,bcoef)
    sinv = read_pert(dirname,scoef)

    Jb = binv*np.sum((xb[0,:,:]-xa[0,:,:])**2)
    Jo = 0
    for i in range(len(obs['val'])):
        x,y,t = obs['X'][i,:]
        Jo += (obs['val'][i]-xa[t,y,x])**2
    J = Jb + sinv*Jo
    return(J)

def read_pert(dirname,filecoef):
    fs = open(os.path.join(dirname,filecoef))
    s = fs.readline().strip().split()
    fs.close()
    return(float(s[2]))

def load_obs(dirname,obsfile):
    allobs = np.genfromtxt(os.path.join(dirname,obsfile))
    coord = np.array(allobs[:,:-1],dtype=int)
    obs = {'X': coord, 'val':allobs[:,-1]}
    return(obs)

def load_exp(dirname,Nens,true_file='state_true_0.nc',pref_ret='state_ret_',prod='Hfil'):
    ft = Dataset(os.path.join(dirname,true_file))
    xt = ft.variables[prod][:]
    ft.close()
    xa = np.zeros((Nens,)+xt.shape)
    for i in range(Nens):
        fname = pref_ret + str(i+1) + '.nc'
        print os.path.join(dirname,fname)
        fx = Dataset(os.path.join(dirname,fname))
        xa[i,:,:,:]=fx.variables[prod][:]
        fx.close()
    return(xa,xt)

def reliability(xt,xa,thr,nbins=20,mask=None,oplot=True):
    if mask is None:
        mask = np.ones(xt.shape,dtype=bool)
    xxt = xt[mask]
    xxa = xa[:,mask]
    nens = xa.shape[0]
    fp = np.zeros(len(xxt)) #forecast probability
    for i in range(len(xxt)):
        fp[i] = np.count_nonzero(xxa[:,i]>thr)
    fp = fp/nens
    freq_tot = np.mean(fp)
    xprob = np.linspace(0,1,nbins+1)
    #xc = 0.5*(xprob[:-1]+xprob[1:])
    xc = np.zeros(nbins)
    xprob[-1]=1.1 #trick to allow strict inequality
    obsf = np.zeros(nbins)
    Np = np.zeros(nbins)
    for i in range(nbins):
        ind, = np.nonzero(np.logical_and(fp>=xprob[i],fp<xprob[i+1]))
        Np[i]=len(ind)
        if Np[i]>0:
            obsf[i] = np.count_nonzero(xxt[ind]>thr)/float(len(ind))
            xc[i] = np.mean(fp[ind])
        else:
            obsf[i] = np.nan
            xc[i] = np.nan

    Np = Np/float(len(xxt))
    
  
    if oplot:
        plt.plot(xc,Np,'o--',color='gray')
        plt.plot([0,1],[0,1],'b')
        plt.plot(xc,obsf,'o-k')
        plt.plot([0,1],[freq_tot]*2,':k')
        #plt.savefig('/home/jbrajard/case-study/2016/fig_aves/2016_04_04/reliability0.png')
        plt.xlabel('forecast probability')
        plt.ylabel('observed frequency')
        plt.show()

    return(obsf,xc,Np)
        
   

def rank_diag(xt,xa,hinit=None,mask=None):
    if hinit is None:
        hinit = np.zeros(Nens+1)
    if mask is None:
        mask = np.ones(xt.shape,dtype=bool)
    N = np.sum(mask)
    xxa = xa[:,mask]
    ninf = np.Inf*np.ones([1,xxa.shape[1]])
    nninf = np.NINF*np.ones([1,xxa.shape[1]])
    xxa = np.concatenate((nninf,xxa,ninf))
    xxt = xt[mask]

    for i in range(len(xxt)):
        l = xxa[:,i]
        l.sort()
        ind = np.searchsorted(l,xxt[i])
        hinit[ind-1]+=1
    
    print 'chi2 test'
    fstat,pvalue=chisquare(hinit)
    print 'stat =',fstat,' ; pvalue = ' , pvalue
    plt.bar(np.arange(Nens+1),hinit)
    #plt.savefig('/home/jbrajard/case-study/2016/fig_aves/2016_04_04/rank0.png')
    plt.show()

    return(hinit)

if __name__ == "__main__":
    Nens = 30
    dirname = '../data/EXP03/output'
    
    #xa,xt = load_exp(dirname,Nens,pref_ret='state_ret_')
    #mask = np.ones(xt.shape,dtype=bool)
    #mask[:-50,:,:]=False
    #hinit = rank_diag(xt,xa,mask=mask)
    #plt.bar(np.arange(Nens+1),hinit)
    #plt.show()
    #obsf,xc, = reliability(xt,xa,thr=0,nbins=20,mask=mask)
#plt.imshow(xt[-1,:,:])
#plt.show()
    J = np.zeros(Nens)
    for i in range(Nens):
        print i
        J[i]= compute_cost(dirname,suff=str(i+1))

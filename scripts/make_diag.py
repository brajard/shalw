from __future__ import print_function 
import os
import sys
import math
sys.path.reverse() #look in anaconda paths first
from netCDF4 import Dataset
import numpy as np
import matplotlib.pyplot as plt
import re
import pandas as pd


def compute_dh(outdir,name,figdir,yi=40,ti=100,figname=None,prod='Hfil'):
    f = Dataset(os.path.join(outdir,name))
    x = f.variables[prod]
    dh = x[1:,yi,:] - x[:-1,yi,:]
    f.close()
    t = np.arange(dh.shape[0])*(1.0/48) #dt=30min
    vmin = np.min(dh.ravel())
    vmax = np.max(dh.ravel())
    fig,ax = plt.subplots(1,2)
    im=ax[0].imshow(dh[:ti],extent=[0,79,t[0],t[ti]],aspect='auto',vmin=vmin,vmax=vmax)
    ax[0].set_xlabel('x')
    ax[0].set_ylabel('time')
    ax[0].set_title('dh/dt')
    ax[1].imshow(dh[ti:],extent=[0,79,t[-ti],t[-1]],aspect='auto',vmin=vmin,vmax=vmax)
    ax[1].set_xlabel('x')
    ax[1].set_ylabel('time')
    fig.subplots_adjust(right=0.8)
    cbar_ax = fig.add_axes([0.8, 0.15, 0.05, 0.7])
    fig.colorbar(im, cax=cbar_ax)
    if figname is None:
        figname = os.path.splitext(name)[0]+'_dh'
    fig.savefig(os.path.join(figdir,figname+'.png'))
#from scipy.stats import chisquare

def recompute_finalcost(outdir,imemb,
                        obs_pref='obs_per_',
                        xa_pref='state_ret_',
                        xt_name='state_true_0',
                        xb_pref='state_bck_',
                        prod = 'Hfil'):
    fa = Dataset(os.path.join(outdir,xa_pref+imemb+'.nc'))
    xa = fa.variables[prod]
    if xb_pref is not None:
        fb = Dataset(os.path.join(outdir,xb_pref+imemb+'.nc'))
        xb = fb.variables[prod]

        
    obs = pd.read_csv(os.path.join(outdir,obs_pref+imemb+'.dat'),
                      sep = ' ',
                      skiprows = 1,
                      names = ['x','y','t','val'])
    
    #innovation (error for obs)
    innov = np.zeros(obs.val.shape)
    for index,row in obs.iterrows():
        x = int(row['x'])
        y = int(row['y'])
        t = int(row['t'])
        modval = xa[t,y,x]
        innov[index] = modval - row['val']
    Jobs = np.sum (np.square(innov))
    

    if xb_pref is not None:    
    #background error
        errb = xa[0,:,:] - xb[0,:,:]
        Jbck = np.sum(np.square(errb.ravel()))


    fa.close()        
    if xb_pref is not None:
        fb.close()

                  


def extract_cost(l):
    """ extract cost info from str 'iter n,simul j,f= cost'
    return typledict (n,j,cost)"""
    m = l.split(',')
    niter = int(m[0].split(' ')[-1])
    nsimul = int(m[1].split(' ')[-1])
    cost = float(m[2].split(' ')[-1].replace('D','E'))
    return{'niter':niter,'nsimul':nsimul,'cost':cost}

def allcost(imemb,logdir,pref='log_',suff='.out'):
    with open(os.path.join(logdir,pref+imemb+suff),'r') as f:
        data = f.read()
    pat = '[ ]*iter[ ]+[0-9]+,[ ]+simul[ ]+[0-9]+,[ ]f=[ ]+[0-9.+D]+' 
    m =re.findall(pat,data)
    allcost = [extract_cost(l) for l in m]
    niter = [c['niter'] for c in allcost]
    cost = [c['cost'] for c in allcost]
    return {'niter':niter,'cost':cost}


def plotcost(allcost,figdir,fname='cost.png'):
    f1,ax = plt.subplots()
    for l in allcost:
        ax.semilogy(l['niter'],l['cost'],color='gray')
    ax.set_xlabel('iteration')
    ax.set_ylabel('cost function')
    f1.savefig(os.path.join(figdir,fname))

def finalcost(imemb,logdir,pref='log_',suff='.out'): 
    with open(os.path.join(logdir,pref+imemb+suff),'r') as f:
        data = f.read()
    m =re.findall('[ ]+f[ ]+=[ ]+[a-zA-z0-9.+]+',data)
    if not len(m) == 2:
        return None
    cost = m[1]
    cost = cost.split()[-1]
    cost = float(cost.replace('D','E'))
    return(cost)

def hasConverged(imemb,logdir,pref='log_',suff='.out'):
    """
    look in the log file if the minimization has converged
    imemb : member identification
    """
    try: 
        imemb=str(imemb)
    except ValueError:
        print('not a valid member value')
        raise
    with open(os.path.join(logdir,pref+imemb+suff),'r') as f:
        data = f.read()
    m =re.findall('[ ]+f[ ]+=[ ]+[a-zA-z0-9.+]+',data)
    if not len(m) == 2:
        return False
    cost = m[1]
    cost = cost.split()[-1]
    cost = float(cost.replace('D','E'))
    return(not math.isnan(cost))

def listmembers(dirname,pref_ret='state_ret',logdir='..',checkConv=True,pref='log_',suff='.out'):
    """ give the list of members for a given experiment
    if checkConv is True, check in the logdir (path relative to dirname) if the experiment has converged
    """
    lens = [re.split('[_.]',f)[-2] for f in os.listdir(dirname) if pref_ret in f]
    if checkConv:
        lens = [m for m in lens if hasConverged(m,os.path.abspath(os.path.join(dirname,logdir)),pref,suff)]
    return lens

class myexp():
    def __init__(self,dirname,lens=None,true_file='state_true_0.nc',pref_ret='state_ret_',prod='Hfil'):
        self.dirname = dirname
        self.pref_ret = pref_ret
        self.prod = prod
        self.true_file = true_file
        if lens is None:
            self.lens = listmembers(self.dirname,pref_ret=self.pref_ret)
        else:
            self.lens = lens
        
    def __enter__(self):
        print('Load EXP in directory ',os.path.join(self.dirname,self.true_file))
        self.ft = Dataset(os.path.join(self.dirname,self.true_file))
        self.xt = self.ft.variables[self.prod]
    #ft.close()
        self.xa = []#np.zeros((Nens,)+xt.shape)
        self.fx = []
        for ens in self.lens:
            fname = self.pref_ret + ens + '.nc'
        #print os.path.join(dirname,fname)
            self.fx.append(Dataset(os.path.join(self.dirname,fname)))
        #xa[i,:,:,:]=fx.variables[prod][:]
            self.xa.append(self.fx[-1].variables[self.prod])
       
    
        print ("Number of members loaded:",len(self.xa))
        return(self.xa,self.xt,self.ft,self.fx)

    def __exit__(self,type,value,traceback):
        self.ft.close()
        for f in self.fx:
            f.close()
        
def computerms(xa,xt,name=None):
    delta = (xa-xt)**2
    delta= np.reshape(delta,[delta.shape[0],-1])
    rms = np.sqrt(np.mean(delta,axis=1))
    if name is not None:
        tax = np.arange(len(rms))*(1.0/48)
        f1,ax = plt.subplots()
        ax.plot(tax,rms)
        ax.set_xlabel('Days')
        ax.set_ylabel('RMS error')
        f1.savefig(name)
    return(rms)

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
        print(os.path.join(dirname,fname))
        fx = Dataset(os.path.join(dirname,fname))
        xa[i,:,:,:]=fx.variables[prod][:]
        fx.close()
    return(xa,xt)

def Brier_score(xt,xa,thr,nbins=20,tmask=-1):
    #True (obs) field
    xxt = np.reshape(xt[tmask,:,:],-1)
        
    #Analyse (forecast) fiels
    xxa = [np.reshape(xaj[tmask,:,:],-1) for xaj in xa]
    
    #Ensemble size
    nens = len(xxa)
    
    #Number of forecast
    N = xxt.size
    
    #Forecast
    fcast = np.zeros(N)
    
    for i in range(N):
        F = np.array([x[i] for x in xxa])
        fcast[i] = np.sum(F>thr)/float(nens)
    
    # forecast bins
    fp_bins = np.linspace(0,1,nbins+1)
    fp_bins[-1]=1.001

    #forecast probabilities
    fp = np.zeros(nbins)
    
    #observed frequency
    obsf = np.zeros(nbins)
    
    # Number of time fp was predicted
    n = np.zeros(nbins)
    
    #observed climatology
    obsc = np.sum(xxt>thr)

    for i in range(nbins):
        ind, = np.nonzero(np.logical_and(fcast>=fp_bins[i],fcast<fp_bins[i+1]))
        n[i] = ind.size
        if n[i] > 0 :
            obsf[i] = np.sum(xxt[ind]>thr)/float(n[i])
            fp[i] = np.mean(fcast[ind])
    
    #Brier Score

    #UNC
    if obsc==0 or obsc==1:
        UNC = 0.0
    else:
        UNC = (obsc/float(N))*(1-obsc/float(N))
    
    #REL
    REL = np.sum(n*((fp-obsf)**2))/float(N)

    #REF (Resolution in Talagrand)
    REF = np.sum(n*obsf*(1-obsf))/float(N)

    return REL,REF,UNC


    
    
def reliability_fast(xt,xa,thr,nbins=20,tmask=-1,oplot=True):
    #CHANGE : xa is a list of arrays (or netcdf.Variable type)
    if mask is None:
        mask = np.ones(xt.shape,dtype=bool)
        
    #True (obs) field
    xxt = np.reshape(xt[tmask,:,:],-1)
        
    #Analyse (forecast) field
    xxa = [xj[tmask,:,:] for xj in xa]
        
    #Ensemble size
    nens = xxa.size

    #Total number of forecast
                  

    #Analyse (forecast) probability
    fp = np.zeros(len(xxt))
    fp = np.arrays([np.count_nonzero(xxa[:,i]>thr)/nens for i in range(len(xxt))] )
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
        f1,ax = plt.subplots(figsize=(10,5))
        ax.plot(xc,Np,'o--',color='gray')
        ax.plot([0,1],[0,1],'b')
        ax.plot(xc,obsf,'o-k')
        ax.plot([0,1],[freq_tot]*2,':k')
        #plt.savefig('/home/jbrajard/case-study/2016/fig_aves/2016_04_04/reliability0.png')
        ax.set_xlabel('forecast probability')
        ax.set_ylabel('observed frequency')
        f1.savefig('reliability.png')

    return(obsf,xc,Np)
        
   
def rank_diag_fast (xt,xa,hinit=None,tmask=-1):
    #CHANGE : xa is a list of arrays (or netcdf.Variable type)

    Nens = len(xa)
    if hinit is None:
        hinit = np.zeros(Nens+1)
    
#    N = np.sum(mask)
    xxa = [np.reshape(xaj[tmask,:,:],-1) for xaj in xa]

  #  ninf = np.Inf*np.ones([1,xxa.shape[1]])
  #  nninf = np.NINF*np.ones([1,xxa.shape[1]])
  #  xxa = np.concatenate((nninf,xxa,ninf))
    xxt = np.reshape(xt[tmask,:,:],-1)

    print('Number of analysis (forecast)',xxt.size)
    for i in range(len(xxt)):
        l = np.array([x[i] for x in xxa])
  #      l.sort()
        ind = np.sum(l>xxt[i])
#        ind = np.searchsorted(l,xxt[i])
        hinit[ind]+=1
    
    return(hinit)


def rank_diag(xt,xa,hinit=None,mask=None):
    Nens = xa.shape[0]
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
    
    print ('chi2 test')
#    fstat,pvalue=chisquare(hinit)
 #   print 'stat =',fstat,' ; pvalue = ' , pvalue
  #  plt.bar(np.arange(Nens+1),hinit)
    f1,ax = plt.subplots(figsize=(10,5))
    ax.bar(np.arange(Nens+1),hinit)
    ax.plot([0,Nens+1],[float(N)/(Nens+1)]*2,'--r')
    ax.set_xlabel('bins')
    ax.set_ylabel('frequency')
    plt.savefig('rank.png')
#    plt.show()

    return(hinit)


def plot_H(H, fname):
    Nens = H.size
    N = np.sum(H)
    f1,ax = plt.subplots(figsize=(10,5))
    ax.bar(np.arange(Nens),H)
    ax.plot([0,Nens+1],[float(N)/Nens]*2,'--r')
    ax.set_xlabel('bins')
    ax.set_ylabel('frequency')
    plt.savefig(fname,dpi=200)

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
        print (i)
        J[i]= compute_cost(dirname,suff=str(i+1))

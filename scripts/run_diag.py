#!/usr/bin/env python
"""
Run basic diagnostics for one experiment
Just specifity datadir,expname and diagdic in the beginning of the script
"""

from __future__ import print_function 
import os
import sys
sys.path.reverse() #look in anaconda paths first
import numpy as np
import matplotlib.pyplot as plt
import make_diag as diag
from importlib import reload
reload(diag)
import re
from mytools import makedirs_sure

datadir = '/data/jbrajard/data/'
#datadir = '/data/ealexiou'
expname = 'EXP11'
diagdic = {
#    'rank_diag':{'tmask':1460, 'mask':80, 'nens':21},
#    'cost':None,
    'rms':None
    }

#For rank_diag
#tmask : time slice to do the time extraction 
#mask : number of point to take by dimension (so the total number of point is mask**2
#nens : size of the ensemble, if lens is bigger, accumalate the score as it is different realizations

#figdir = '/data/jbrajard/data/eva_figs/fig'
figdir = os.path.join(datadir,expname,'fig')

makedirs_sure(figdir)

#Directory where the experiment is stored
expdir = os.path.join(os.path.join(datadir,expname,'output'))

#Definition of the ensemble
lens = diag.listmembers(expdir)
lens = lens[:21]
for d in diagdic:
    dv = diagdic[d]
    if d == 'rank_diag':
        print('--- compute rank diagram ---')    
        
        tmask = dv['tmask']
        H = None
        if 'nens' in dv:
            nens = dv['nens']
            Lens = [ lens[i*nens : (i+1)*nens] for i in range(len(lens)//nens) ]
        else:
            Lens = [lens]
        for lreal in Lens:
            with diag.myexp(expdir,lens=lreal) as (xa,xt,ft,fx):
                if 'mask' in dv :
                    stepi = xt.shape[1]//dv['mask']
                    stepj = xt.shape[2]//dv['mask']
                    indxi = slice(0,xt.shape[1],stepi)
                    indxj = slice(0,xt.shape[2],stepj)
                    xaa = [x[[tmask],indxi,indxj] for x in xa]
                    xtt = xt[[tmask],indxi,indxj]
                else:
                    xaa = [x[[tmask],:,:] for x in xa]
                    xtt = xt[[tmask],:,:]
                H = diag.rank_diag_fast(xtt,xaa,hinit=H)
                print(sum(H))
        diag.plot_H(H,os.path.join(figdir,'rank_diag.png'))

    elif d == 'rms':
        print('--- compute RMS ---')    
        with diag.myexp(expdir,lens=lens) as (xa,xt,ft,fx):
            xam = np.mean(xa,axis=0)
            xtt = np.array(xt)
        diag.computerms(xam,xtt,os.path.join(figdir,'rms_a_time.png'))

    elif d == 'cost':
        print('--- compute final cost ---')
        cost = [diag.finalcost(imemb,os.path.join(datadir,expname)) for imemb in lens]

    else:
        print('--- diag ' + d + ' not implemented ---')

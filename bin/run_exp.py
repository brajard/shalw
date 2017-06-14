#!/usr/bin/env python

import subprocess as sp
import os
import sys
import importlib
import getopt

#Init des paths
bindir = os.path.dirname(__file__)
srcdir = os.path.abspath(os.path.join (bindir, "../src"))
cdir = os.getcwd()
datadir =  os.path.abspath(os.path.join (bindir, "../data"))
scriptdir =  os.path.abspath(os.path.join (bindir, "../scripts"))
sys.path.append(srcdir)
sys.path.append(scriptdir)
from mytools import makedirs_sure, silentremove, make_namelist, make_error_coef, run_usage
exname = 'shalw'
exefile = os.path.join(os.path.abspath(bindir), exname)

tocompile = True




## Input args
configfile = 'Config_forw'

try:
    opts,args = \
        getopt.getopt(sys.argv[1:],"hc:",
                      ["help","config=","exp=","suff=","nrun=","save_freq=","bck_state=","bck","no-compile"])
except getopt.GetoptError:
    run_usage()
    sys.exit(2)

exp_name = '02'
suff = ''
tocompile = True
nrun = None
bck_state = None
save_freq=0
bck = False

for opt,arg in opts:
    if opt in ("-h","--help"):
        run_usage()
        sys.exit()
    elif opt in  ("-c","--config"):
        configfile = arg
    elif opt in ("--exp"):
        exp_name = arg
    elif opt in ("--bck"):
        bck = True
    elif opt in ("--bck_state"):
        bck_state = arg
    elif opt in ("--suff"):
        suff = arg
    elif opt in ("--no-compile"):
        tocompile = False	
    elif opt in ("--nrun"):
        nrun = int(arg)
    elif opt in ("--save_freq"):
        save_freq=int(arg)

print ("config class used : "+configfile)
print ("nrun="+str(nrun))

cfg = importlib.import_module('config')
strclass = "cfg." + configfile + "(exp_name,suff,tocompile"
if nrun is not None : #only for free model run
    strclass += ",nrun=nrun,save_freq=save_freq,bck=bck"
if bck_state is not None:
    strclass += ",bck_state=bck_state"

strclass += ")"
print (strclass)
conf = eval(strclass)
exp_name = conf.exp_name
namelist= conf.namelist
yao_opt = conf.yao_opt


#from config_renorm import exp_name,yao_opt,namelist


expdir = os.path.join(datadir,'EXP' + exp_name)
yao_opt['forw']=os.path.join(scriptdir,yao_opt['forw'])
yao_opt['gen_opt']=yao_opt['gen_opt']+' -x ' + exefile
namelist['indir']=os.path.join(expdir,namelist['indir'])
namelist['outdir']=os.path.join(expdir,namelist['outdir'])



#print cdir
#print bindir
#print srcdir
print (expdir)

#prepare paths
makedirs_sure(expdir)
indir = namelist['indir']
outdir = namelist['outdir']
makedirs_sure(indir)
makedirs_sure(outdir)


#COMPILATION OF THE PROJECT
#print yao_opt

#if (type(yao_opt['compile']) is str) and yao_opt['compile'] is 'auto':
#    print 'compile auto ',True
#    yao_opt['compile']=True

if yao_opt['compile']:
    silentremove(exefile)
    ret=sp.call(
        ['cd ' + srcdir + ' ; ../bin/yao92 ' + yao_opt["gen_opt"] + ' shalw ; cd ' + cdir],
        shell=True)


#GENERATION OF NAMELIST
namefile = os.path.join(indir,conf.namefile)
make_namelist(namefile,namelist)
make_error_coef(indir,namelist)

#EXECUTE THE PROJECT
if os.path.isfile(exefile):
    
    strcmd = ['cd ' + indir + ' ; ' +
              exefile + ' namelist ' + 
              namefile + ' -i ' + yao_opt["forw"] + 
              ' ; cd ' + cdir]
    print (strcmd)
    sp.call(
        strcmd,
        shell=True)

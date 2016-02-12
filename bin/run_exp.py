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




## Input args
configfile = 'config'

try:
    opts,args = getopt.getopt(sys.argv[1:],"hc:",["help","config="])
except getopt.GetoptError:
    run_usage()
    sys.exit(2)

for opt,arg in opts:
    if opt in ("-h","--help"):
        run_usage()
        sys.exit()
    elif opt in  ("-c","--config"):
        configfile = arg


print "config file used : "+configfile

mconf = importlib.import_module(configfile)
exp_name = mconf.exp_name
yao_opt = mconf.yao_opt
namelist = mconf.namelist
#from config_renorm import exp_name,yao_opt,namelist


expdir = os.path.join(datadir,'EXP' + exp_name)
yao_opt['forw']=os.path.join(scriptdir,yao_opt['forw'])
yao_opt['gen_opt']=yao_opt['gen_opt']+' -x ' + exefile
namelist['indir']=os.path.join(expdir,namelist['indir'])
namelist['outdir']=os.path.join(expdir,namelist['outdir'])



#print cdir
#print bindir
#print srcdir
print expdir

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
namefile = os.path.join(indir,"namelist")
make_namelist(namefile,namelist)
make_error_coef(indir,namelist)

#EXECUTE THE PROJECT
if os.path.isfile(exefile):
    
    strcmd = [exefile + ' namelist ' + namefile + ' -i ' + yao_opt["forw"] ]
    print strcmd
    sp.call(
        strcmd,
        shell=True)

#!/usr/bin/env python

import subprocess as sp
import os
import sys

#Init des paths
bindir = os.path.dirname(__file__)
srcdir = os.path.abspath(os.path.join (bindir, "../src"))
cdir = os.getcwd()
datadir =  os.path.abspath(os.path.join (bindir, "../data"))
scriptdir =  os.path.abspath(os.path.join (bindir, "../scripts"))
sys.path.append(srcdir)
from mytools import makedirs_sure, silentremove
exname = 'shalw'
exefile = os.path.join(os.path.abspath(bindir), exname)

#global parameters

yao_opt = {
'compile':True, 
'gen_opt':'-p  -x '+exefile,
'forw': os.path.join(scriptdir,'forw.i')
}

exp_name = '00'
expdir = os.path.join(datadir,'EXP' + exp_name)


#print cdir
#print bindir
#print srcdir
print expdir

#prepare paths
makedirs_sure(expdir)
indir = os.path.join(expdir,"input")
outdir = os.path.join(expdir,"output")
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

#EXECUTE THE PROJECT
if os.path.isfile(exefile):
    namefile = os.path.join(indir,"namelist")
    strcmd = [exefile + ' namelist ' + namefile + ' -i ' + yao_opt["forw"] ]
    print strcmd
    sp.call(
        strcmd,
        shell=True)

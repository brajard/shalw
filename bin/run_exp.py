#!/usr/bin/env python

import subprocess as sp
import os

#global parameters

yao_opt = {
'compile':True, 
'gen_opt':'-p +O3', 
}



#Init des paths
bindir = os.path.dirname(__file__)
srcdir = os.path.abspath(os.path.join (bindir, "../src"))
cdir = os.getcwd()
#print cdir
#print bindir
#print srcdir


#COMPILATION OF THE PROJECT
#print yao_opt
if yao_opt['compile']:
    sp.call(
        ['cd ' + srcdir + ' ; ../bin/yao92 ' + yao_opt["gen_opt"] + ' shalw ; cd ' + cdir],
        shell=True)



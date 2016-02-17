#!/usr/bin/env python


import subprocess as sp
import os
import sys
import importlib

#Size of the ensemble
Nens = 2

bindir = os.path.dirname(__file__)
exfile = 'run_exp.py'
datadir =  os.path.abspath(os.path.join (bindir, "../data"))


#Forward to generate observations
optline = ' -c Config_forw --suff=0'
sp.call([os.path.join(bindir,exfile) + ' ' + optline],shell=True)

#Assim
for i in range(Nens):
    optline = ' -c Config_var --no-compile --suff=' + str(i+1)
    sp.call([os.path.join(bindir,exfile) + ' ' + optline],shell=True)


#!/usr/bin/env python

import time
import subprocess as sp
import os
import sys
import importlib

#Name of experiment
exp_name = '07'

bindir = os.path.dirname(__file__)
exfile = 'run_exp.py'
datadir =  os.path.abspath(os.path.join (bindir, "../data"))
expdir = os.path.join(datadir,'EXP' + exp_name)


start = time.time()

#Forward to generate observations
print 'Running forward model with balanced velocity...'
optline = ' -c Config_forw --suff=b --exp='+exp_name
logfile = open(os.path.join(expdir,'log_b.out'),'w')
sp.call([os.path.join(bindir,exfile) + ' ' + optline],shell=True,stdout = logfile, stderr=sp.STDOUT)
end = time.time()



print 'Running forward model with total intial field...(elapsed time = ', end-start, 'sec)'
optline = ' -c Config_forw_nb --suff=nb --exp='+exp_name
logfile = open(os.path.join(expdir,'log_nb.out'),'w')
sp.call([os.path.join(bindir,exfile) + ' ' + optline],shell=True,stdout = logfile, stderr=sp.STDOUT)
end = time.time()

print 'Finished (elapsed time = ', end-start, 'sec)'

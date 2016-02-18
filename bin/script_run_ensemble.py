#!/usr/bin/env python

import time
import subprocess as sp
import os
import sys
import importlib

#Size of the ensemble
Nens = 20

#Name of experiment
exp_name = '02'

bindir = os.path.dirname(__file__)
exfile = 'run_exp.py'
datadir =  os.path.abspath(os.path.join (bindir, "../data"))
expdir = os.path.join(datadir,'EXP' + exp_name)


start = time.time()

#Forward to generate observations
print 'Running forward model...'
optline = ' -c Config_forw --suff=0 --exp='+exp_name
logfile = open(os.path.join(expdir,'log_0.out'),'w')
sp.call([os.path.join(bindir,exfile) + ' ' + optline],shell=True,stdout = logfile, stderr=sp.STDOUT)

end = time.time()

#Assim
print 'Running EnsVar...(elapsed time = ', end-start, 'sec)'
proc=[]
for i in range(Nens):
    logfile = open(os.path.join(expdir,'log_'+str(i+1)+'.out'),'w')
    optline = ' -c Config_var --no-compile --suff=' + str(i+1) + ' --exp='+exp_name
    proc.append(sp.Popen([os.path.join(bindir,exfile) + ' ' + optline],
             shell=True,stdout = logfile, stderr=sp.STDOUT))

print 'Waiting for process to end...'
for p in proc:
    p.wait()
    print 'Member pid #',p.pid,' terminated'
end = time.time()

print 'Finished (elapsed time = ', end-start, 'sec)'

#!/usr/bin/env python

import time
import subprocess as sp
import os
import sys
import importlib

#Size of the ensemble
Nens = 30

#Name of experiment
exp_name = '02'

bindir = os.path.dirname(__file__)
exfile = 'run_exp.py'
datadir =  os.path.abspath(os.path.join (bindir, "../data"))
expdir = os.path.join(datadir,'EXP' + exp_name)
batchdir = os.path.join(bindir,'batchdir')

start = time.time()

#Generation of qsub file
def qsubfile(cmdline,fname,output,header='batch_header.sh'):
    f = open(fname,'w')
    fh = open(header,'r')
    for l in fh:
        f.write(l)
    fh.close()
    f.write('#PBS -o ' + output +'\n')
    f.write(cmdline)
    f.close()
    #os.system('chmod u+x '+fname)

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
    logfile = os.path.join(expdir,'log_'+str(i+1)+'.out')
    optline = ' -c Config_var --no-compile --suff=' + str(i+1) + ' --exp='+exp_name
    fname = os.path.join(batchdir,'batch_' + str(i+1) + '.sh')
    qsubfile(os.path.abspath(os.path.join(bindir,exfile)) + ' ' + optline,fname,logfile)
    os.system('qsub '+fname)
    
#    proc.append(sp.Popen([os.path.join(bindir,exfile) + ' ' + optline],
#             shell=True,stdout = logfile, stderr=sp.STDOUT))

#print 'Waiting for process to end...'
#for p in proc:
#    p.wait()
#    print 'Member pid #',p.pid,' terminated'
#end = time.time()

#print 'Finished (elapsed time = ', end-start, 'sec)'

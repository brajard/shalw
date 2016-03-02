import numpy as np
import os
import errno

def rankdiag(q,xtrue,xest):
    v = np.percentile(xtrue,q)
    vret = np.zeros(len(q)-1)
    for i in range(0,len(q)-1):
        vret[i]=((v[i] <= xest) & (xest < v[i+1])).sum()
    vret[0]+=(xest < v[0]).sum()
    vret[-1]+=(v[-1] <= xest).sum()
    return(vret)

def makedirs_sure(path):
    try:
        os.makedirs(path)
    except OSError as exception:
        if exception.errno != errno.EEXIST:
            raise

def silentremove(filename):
    try:
        os.remove(filename)
    except OSError as e: # this would be "except OSError, e:" before Python 2.6
        if e.errno != errno.ENOENT: # errno.ENOENT = no such file or directory
            raise # re-raise exception if a different error occured


def make_namelist(filename,namelist):
    f = open(filename,'w')
    for par,value in namelist.items():
        f.write(par + ' ' + value + '\n')
    f.close()

def make_error_coef(indir,namelist):
    f = open(os.path.join(indir,"bcoef.i"),'w')
    if float(namelist['bckper'])>0:
        bcoef = 1.0/(float(namelist["bckper"])**2)
        f.write('set_bcoef Hfil ' + str(bcoef) + '\n')
    f.close()
    f = open(os.path.join(indir,"scoef.i"),'w')
    if float(namelist['obsper'])>0:
        scoef = 1.0/(float(namelist["obsper"])**2)
        f.write('set_scoef Hfil ' + str(scoef) + '\n')
    f.close()

def run_usage():
    print "\nrun_exp.py [OPTIONS]\n"
    print "run_exp.py run a assimilation experiment on shallow-water"
    print "\nOPTIONS"
    print "\t -h, --help"
    print "\t print this help message and exit\n"
    print "\t -c [FILENAME] --config=[FILENAME]"
    print "\t use the file specified in [FILENAME] as configuration file (default is 'config')\n"
    print "\t --suff=[SUFFIXE]"
    print "\t suffix of all output files (default is '')\n"
    print "\t --no-compile"
    print "\t the source code is not compiled and the executable (bin/shalw) is used (bu default, the source is recompiled\n"

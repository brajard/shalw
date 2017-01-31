import os

class Config:
    def __init__(self,exp_name,tocompile,suff):
        self.exp_name=exp_name
        self.yao_opt = {
            'compile':tocompile, 
            'gen_opt':'+O3'}
        self.namelist = {
            'indir':'input',
            'outdir':'output',
            'obsper':'1',
            'bckper':'10',
            }
        self.namefile = 'namelist_'+suff



class Config_forw (Config):
    def __init__ (self,exp_name='02',suff='',tocompile=True):
        Config.__init__(self,exp_name,tocompile,suff)
        self.yao_opt['forw']='forw.i'
        self.namelist['out_true']='state_true_' + suff + '.nc'
        self.namelist['out_obs']='obs_per_' + suff + '.dat'
        self.namelist['out_init']='snapshot_bck_' + suff + '.nc'
        self.namelist['bck_state']='snapshot_10.nc'
        self.namelist['obsfile']='obs.dat'

class Config_forw_nb (Config_forw):
    def __init__ (self,exp_name='02',suff='',tocompile=True):
        Config_forw.__init__(self,exp_name,suff,tocompile)
        self.yao_opt['forw']='forw_nb.i'

class Config_var (Config):
    def __init__ (self,exp_name='02',suff='',tocompile=True,bck_state='snapshot_bck_0.nc'):
        Config.__init__(self,exp_name,tocompile,suff)
        self.yao_opt['forw']='assim.i'
        self.namelist['indir']='output'
        self.namelist['obsfile']='obs_per_0.dat'
        self.namelist['bck_state']=bck_state
        self.namelist['out_true']='state_true_' + suff + '.nc'
        self.namelist['out_bck']='state_bck_' + suff + '.nc'
        self.namelist['out_ret']='state_ret_' + suff + '.nc'
        self.namelist['out_obs']='obs_per_' + suff + '.dat'

class Config_var_nb (Config):
    """
    Config class to be used in an assimilation without background, to specify another snapshot as a first guess
    """
    def __init__ (self,exp_name='02',suff='',tocompile=True,bck_state='snapshot_bck_0.nc'):
        Config.__init__(self,exp_name,tocompile,suff)
        self.yao_opt['forw']='assim.i'
        self.namelist['indir']='output' #modified from parent class
        self.namelist['bckper']='0' #modified from parent class
        self.namelist['obsfile']='obs_per_0.dat'
        self.namelist['bck_state']=bck_state
        self.namelist['out_true']='state_true_' + suff + '.nc'
        self.namelist['out_bck']='state_bck_' + suff + '.nc'
        self.namelist['out_ret']='state_ret_' + suff + '.nc'
        self.namelist['out_obs']='obs_per_' + suff + '.dat'
        


class Config_freerun (Config):
    def __init__ (self,exp_name='04',suff='',tocompile=True,nrun=1,dfile = '../src/shalw.d',dt=1800,save_freq=0,bck=False,bck_state='snapshot_bck_0.nc'):
        #nrun : number of integrated time
        Config.__init__(self,exp_name,tocompile,suff)
        self.indir = os.path.abspath(os.path.join('../data','EXP' + exp_name,self.namelist['indir']))
        self.suff=suff
        self.yao_opt['forw']='spinup.i'
        self.namelist['bck_state']=None
	if bck :
		self.namelist['bck_state']=bck_state
        runtime = self.get_runtime(dfile)
        self.runtime = runtime
        totaltime = self.get_totaltime(runtime,nrun,dt)
        self.namelist['out_true']='state_' + totaltime["str"] + '_' + suff + '.nc'
        self.create_loop(nrun,save_freq)

    def create_loop(self,nrun,save_freq=0):
        print 'nrun=',str(nrun)
        file = open(os.path.join(self.indir,'loop.i'),'w')
        if self.namelist['bck_state'] is not None:
            file.write('xload_init '+self.namelist['bck_state']+'\n')
        for i in range(nrun):
            file.write('set_modeltime 0\n')
            file.write('forward\n')
            if save_freq>0 and i%(save_freq)==0:
                fname = os.path.join(self.indir,'..',self.namelist['outdir'],'state_' + str(i) + '_' + self.suff + '.nc')
                file.write('xsavenc '+ os.path.abspath(fname) + ' state ' + str(self.runtime) + '\n')
            file.write('saveinit\n')
        file.close()

    def get_runtime(self,dfile):
        file = open(dfile,'r')
        runtime = 0
        for L in file:
            l = L.split()
            if len(l)>=3 and ('defval' in l) and ('SZT' in l) and  ('/' not in l[0]) and ('#' not in l[0]):
                runtime = int(l[2])
                break
        file.close()
        return(runtime)
    
    def get_totaltime(self,runtime,nrun,dt):
        totaltime = dict()
        Ts = runtime*nrun*dt
        totaltime['sec']=Ts
        Th = Ts/3600
        totaltime['hours']=Th
        Td = Th/24
        totaltime['days']=Td
        Tm = Td/30
        totaltime['months']=Tm
        Ty = Tm/12
        totaltime['year']=Ty
        
        sline = ''
        if (Ty>0):
            sline=str(Ty)+'y'
            totaltime['str']=sline
            if (Tm-12*Ty > 0):
                totaltime['str']=sline + '_' + str(Tm-12*Ty) +'m'
            return(totaltime)

        if (Tm>0):
            sline=str(Tm)+'m'
            totaltime['str']=sline
            if (Td -30*Tm > 0):
                totaltime['str']=sline + '_' + str(Td-30*Tm) + 'd'
            return(totaltime)

        if(Td>0):
            sline=str(Td)+'d'
            totaltime['str']=sline            
            if (Th - 30*Td >0):
                totaltime['str']=sline+ '_' + str(Th-30*Td) + 'h'
            return(totaltime)
        return(totaltime)
                      
                      
                      
        
        

        

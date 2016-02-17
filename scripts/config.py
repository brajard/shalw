class Config:
    def __init__(self,exp_name,tocompile):
        self.exp_name=exp_name
        self.yao_opt = {
            'compile':tocompile, 
            'gen_opt':'+O3 -p'}
        self.namelist = {
            'indir':'input',
            'outdir':'output',
            'obsfile':'obs.dat',
            'obsper':'1',
            'bckper':'10',
            }


class Config_forw (Config):
    def __init__ (self,exp_name='02',suff='',tocompile=True):
        Config.__init__(self,exp_name,tocompile)
        self.yao_opt['forw']='forw.i'
        self.namelist['out_true']='state_true_' + suff + '.nc'
        self.namelist['out_obs']='obs_per_' + suff + '.dat'
        self.namelist['out_init']='snapshot_bck_' + suff + '.nc'
        self.namelist['bck_state']='snapshot_10.nc'
        
class Config_var (Config):
    def __init__ (self,exp_name='02',suff='',tocompile=True):
        Config.__init__(self,exp_name,tocompile)
        self.yao_opt['forw']='assim.i'
        self.namelist['indir']='output'
        self.namelist['obsfile']='obs_per_0.dat'
        self.namelist['bck_state']='snapshot_bck_0.nc'
        self.namelist['out_true']='state_true_' + suff + '.nc'
        self.namelist['out_bck']='state_bck_' + suff + '.nc'
        self.namelist['out_ret']='state_ret_' + suff + '.nc'
        self.namelist['out_obs']='obs_per_' + suff + '.dat'

#global parameters
exp_name = '00'

yao_opt = {

'compile':True, 
'gen_opt':''             ,
#'forw': os.path.join(scriptdir,'forw.i')
'forw':'forw.i'
}

namelist = {
#indir':os.path.join(expdir,"input"),
#outdir':os.path.join(expdir,"output"),
'indir':'input',
'outdir':'output',
'obsfile':'obs.dat',
'bck_state':'snapshot_10.nc',
'out_true':'state_true.nc',
'out_obs':'obs_per.dat',
'obsper':'1',
'bckper':'10',
}

yao_opt['gen_opt']='+O3 -p'

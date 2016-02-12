#global parameters
exp_name = '02'

yao_opt = {

'compile':True, 
'gen_opt':'+O3 -p'             ,
#'forw': os.path.join(scriptdir,'forw.i')
'forw':'assim.i'
}

namelist = {
#indir':os.path.join(expdir,"input"),
#outdir':os.path.join(expdir,"output"),
'indir':'input',
'outdir':'output',
'obsfile':'obs.dat',
'bck_state':'snapshot_10.nc',
'out_true':'state_true.nc',
'out_bck':'state_bck.nc',
'out_ret':'state_ret.nc',
'out_obs':'obs_per.dat',
'obsper':'1',
'bckper':'10',
}



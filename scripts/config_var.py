#global parameters
try:
    exp_name
except NameError:
    exp_name = '02'

try:
    suff
except NameError:
    suff = ''

try:
    tocompile
except NameError:
    tocompile=True

yao_opt = {

'compile':tocompile, 
'gen_opt':'+O3 -p'             ,
#'forw': os.path.join(scriptdir,'forw.i')
'forw':'assim.i'
}

namelist = {
#indir':os.path.join(expdir,"input"),
#outdir':os.path.join(expdir,"output"),
'indir':'output',
'outdir':'output',
'obsfile':'obs_per_0.dat',
'bck_state':'snapshot_bck_0.nc',
'out_true':'state_true_' + suff + '.nc',
'out_bck':'state_bck_' + suff + '.nc',
'out_ret':'state_ret_' + suff + '.nc',
'out_obs':'obs_per_' + suff + '.dat',
'obsper':'1',
'bckper':'10',
}



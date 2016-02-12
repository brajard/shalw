#global parameters
exp_name = '01'


yao_opt = {

'compile':True, 
'gen_opt':''             ,
#'forw': os.path.join(scriptdir,'forw.i')
'forw':'renorm.i'
}

namelist = {
#indir':os.path.join(expdir,"input"),
#outdir':os.path.join(expdir,"output"),
'indir':'input',
'outdir':'output',
'obsfile':'obs10.dat',
'bck_state':'snapshot_10.nc',
'out_true':'state_true.nc',
'grad_t':'state_grad.nc',
'out_0':'state_0.nc',
'out_a':'state_a.nc',
'obsper':'0',
'bckper':'0',
}

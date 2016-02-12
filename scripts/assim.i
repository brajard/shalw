straj

xivg dt 1800
xivg dx 20000
xivg dy 20000

xivg pcor 3.5e-5
xivg grav 0.02
xivg dissip 2e-7
xivg alpha 0.025
xivg hmoy 500
xivg beta 2.11e-11

#for wind forcing
xivg rho0 1000

#for diffusion
xivg nu 0.72
xwind 0.15
set_modeltime 0
xread_namelist
print_namelist

read_lobs auto
xload_init auto


cost lms 0.5

set_balanced true

set_modeltime 0
forward
xsavenc auto state -1 out_true


xdisplay

#Obs perturbation
xivg sigper auto obsper
load_inst scoef.i
load_allobs

xsave_obs auto

#background pertubation
xivg sigper auto bckper
load_inst bcoef.i
load_bck

set_modeltime 0

forward
xsavenc auto state -1 out_bck

#Assimil
setm_impres 6
setm_io 6
setm_mode 0
#set_nbiter 100
set_nbiter 25
setm_nsim 50
set_nbextl 1
#SET_PCOEF 0
setm_dxmin 1.0e-8
setm_epsg 1.0e-8
setm_ddf1 10.0
RUNM
xsavenc auto state -1 out_ret


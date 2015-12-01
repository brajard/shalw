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
#goto TEST_DF
goto INIT

TEST_DF
testdf 10 10 1 4 r %0.000001 0.0001
goto fin
#testdf 50 10 1 10 1 %0.000001 0.0001

INIT
xgauss 0 15000 15000
#xwind 0.015
xwind 0.15
#xporte 15 15000 15000
#xcos 15 8000 8000
xdisplay
set_modeltime 0
read_lobs obs.dat
#xload_init snapshot.nc

goto SPINUP
#goto FORW1

goto EXP_JUM
goto TEST_OF

FORW1

forward
xdisplay
xsavenc state_true.nc state
goto fin
#goto ADJOINT
goto RENORM

goto fin

SPINUP
forward
xsavenc state_tmp.nc state

xdisplay
saveinit
set_modeltime 0
forward
saveinit
set_modeltime 0
forward
saveinit
set_modeltime 0
forward
saveinit
set_modeltime 0
forward
saveinit
set_modeltime 0
forward
saveinit
set_modeltime 0
forward
saveinit
set_modeltime 0
forward
saveinit
set_modeltime 0
forward
saveinit
set_modeltime 0
forward
saveinit
set_modeltime 0
forward
saveinit
set_modeltime 0
forward
saveinit
set_modeltime 0
forward
saveinit
set_modeltime 0
forward
saveinit
set_modeltime 0
forward
saveinit
set_modeltime 0
forward
saveinit
set_modeltime 0
forward
saveinit
set_modeltime 0
forward

xsavenc state_true.nc state
xsavenc snapshot.nc state 1500
goto fin

ADJOINT
read_lobs obs.dat
compute_adjoint
xdisplay
set_iosep
xsavenc grad_true.nc grad

goto 
goto fin




cost lms 0.5
#testof 0.02 10 10 8
print_cost ON

setm_impres 2
setm_io 6
setm_mode 0
#set_nbiter 100
set_nbiter 20
setm_nsim 20
setm_dxmin 1.0e-12
setm_epsg 1.0e-12
setm_ddf1 1.0

RENORM
cost lms 0.5
#testof 0.02 10 10 8
print_cost ON

setm_impres 2
setm_io 6
setm_mode 0
#set_nbiter 100
set_nbiter 20
setm_nsim 20
setm_dxmin 1.0e-12
setm_epsg 1.0e-12
setm_ddf1 1.0

compute_adjoint
xset_maxiter 0
renorm
set_sol
set_modeltime 0
FORWARD
xsavenc state_0.nc state
xgauss 15 250000 25000 250000 25000
xset_maxiter 20
renorm
set_sol
set_modeltime 0
FORWARD
xsavenc state_a.nc state

goto fin
goto TEST_OF

goto fin

TEST_OF

read_lobs obs.dat
load_allobs
#xgauss 0 15000 15000
saveinit

#goto M1QN3
testof 1 1 10 15 0

goto fin


RUN
xsavenc state_4dvar.nc state

goto fin

xdisplay
#savegrad grad.dat

xsavenc grad.nc grad
xsavenc state.nc state

#xsavestate state.dat
#savestate Hfil 1 ij 5% A 3 ./HfilA
#savestate Hfil 1 ij 301 A 0 ./HfilAobs


M1QN3
setm_impres 3
setm_io 6
setm_mode 0
#set_nbiter 100
set_nbiter 20
setm_nsim 20
setm_dxmin 1.0e-12
setm_epsg 1.0e-12
setm_ddf1 1.0

RUNM
xsavenc state_4dvar.nc state

goto fin

EXP_JUM
forward
saveinit
set_modeltime 0
forward

xsavenc state_true.nc state
xdisplay

xperturb 300 0.1
xdisplay
outoobs Hfil 1 301
xsavenc obs.nc state 300
set_scoef Hfil 100

xperturb 0 1
outoebx Hfil 1 1
set_modeltime 0
forward
xdisplay
xsavenc state_bck.nc state
set_bcoef Hfil 1


setm_impres 3
setm_io 6
setm_mode 0
#set_nbiter 100
set_nbiter 20
setm_nsim 20
setm_dxmin 1.0e-12
setm_epsg 1.0e-12
setm_ddf1 1.0

RUNM
xsavenc state_4dvar.nc state

goto fin



fin




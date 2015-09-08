straj

xivg dt 1.50e3
xivg dx 5000
xivg dy 5000


#xivg dt 1000
#xivg dx 4000
#xivg dy 4000
xivg pcor 0.0001
xivg grav 0.01
xivg dissip 0.00001
#xivg dissip 0
xivg alpha 0.15
xivg hmoy 100

#xgauss 15 25000 2500 25000 2500

#xgauss 15 200000 12500 200000 12500
xgauss 15 250000 25000 250000 25000

#xvitgeo

#testdf 10 10 1 2 1 %0.000001 0.0001
#testdf 50 10 1 10 1 %0.000001 0.0001
xdisplay
set_modeltime 0
FORWARD
#xdisplay
#set_modeltime 0
#FORWARD
xdisplay
#testdf 50 50 1 3 R %0.000001 0.0001

set_iosep



xsavenc state_true.nc state
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

read_lobs obs.dat
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
goto fin
fin

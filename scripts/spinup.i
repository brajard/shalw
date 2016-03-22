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

set_balanced false

load_inst loop.i

xsavenc auto state 0 out_true



//#define LIN

defval SZX 100
defval SZY 100
defval SZU 1 
defval SZT 300
defval SZA 301
//defval K_LIN 
 
hat_name "shalw.h" "renorm.h" 

option o_gradtest
//option o_parallel
option o_m1qn3
 
exec disp_option

traj Toce M SZU SZT
traj Tcst M 0 1
//traj Toce2 M SZU 0 2 SZT

space Soce M SZX SZY Toce
space Soce_cst M SZX SZY Tcst

modul Hfil  space Soce input 3 output 1 tempo cout target
modul Ufil  space Soce input 3 output 1 tempo
modul Vfil  space Soce input 3 output 1 tempo
modul Hphy  space Soce input 3 output 1 tempo
modul Uphy  space Soce input 7 output 1 tempo
modul Vphy  space Soce input 7 output 1 tempo
modul Hz    space Soce input 4 output 1 tempo
modul Vit   space Soce input 5 output 1 tempo
modul Vor   space Soce input 4 output 1 tempo
modul Lamu  space Soce input 6 output 1 tempo
modul Lamv  space Soce clonof Lamu
modul Gradx space Soce input 2 output 1 tempo
modul Grady space Soce clonof Gradx
modul Mcu   space Soce input 5 output 1 tempo
modul Mcv   space Soce clonof Mcu
modul Tau_forx space Soce input 3 output 1 tempo
modul Tau_fory space Soce clonof Tau_forx
modul Taux  space Soce_cst noward output 1
modul Tauy  space Soce_cst noward output 1
modul Difu  space Soce  input 5 output 1
modul Difv  space Soce  clonof Difu

ctin Difu 1 from Uphy 1 i+1 j   t-1
ctin Difu 2 from Uphy 1 i-1 j   t-1
ctin Difu 3 from Uphy 1 i   j   t-1
ctin Difu 4 from Uphy 1 i   j+1 t-1
ctin Difu 5 from Uphy 1 i   j-1 t-1

ctin Difv 1 from Vphy 1 i+1 j   t-1
ctin Difv 2 from Vphy 1 i-1 j   t-1
ctin Difv 3 from Vphy 1 i   j   t-1
ctin Difv 4 from Vphy 1 i   j+1 t-1
ctin Difv 5 from Vphy 1 i   j-1 t-1


ctin Tau_forx 1 from Taux 1 i   j
ctin Tau_forx 2 from Hphy 1 i   j t-1
ctin Tau_forx 3 from Hphy 1 i+1 j t-1

ctin Tau_fory 1 from Tauy 1 i   j
ctin Tau_fory 2 from Hphy 1 i   j-1 t-1
ctin Tau_fory 3 from Hphy 1 i   j   t-1

ctin Lamu 1 from Vor  1 i-1 j   t-1
ctin Lamu 2 from Vor  1 i   j t-1
ctin Lamu 3 from Uphy 1 i-1 j-1 t-1
ctin Lamu 4 from Uphy 1 i-1 j   t-1
ctin Lamu 5 from Uphy 1 i   j-1 t-1
ctin Lamu 6 from Uphy 1 i   j   t-1

ctin Lamv 1 from Vor  1 i   j   t-1
ctin Lamv 2 from Vor  1 i   j+1 t-1
ctin Lamv 3 from Vphy 1 i   j   t-1
ctin Lamv 4 from Vphy 1 i   j+1 t-1
ctin Lamv 5 from Vphy 1 i+1 j   t-1
ctin Lamv 6 from Vphy 1 i+1 j+1 t-1

ctin Hphy 1 from Hfil 1 i   j   t-1
ctin Hphy 2 from Mcu  1 i   j   t
ctin Hphy 3 from Mcv  1 i   j   t

ctin Hz   1 from Hphy 1 i   j   t
ctin Hz   2 from Hphy 1 i+1 j   t
ctin Hz   3 from Hphy 1 i   j-1 t
ctin Hz   4 from Hphy 1 i+1 j-1 t

ctin Gradx 1 from Vit 1 i+1 j   t-1
ctin Gradx 2 from Vit 1 i   j   t-1

ctin Grady 1 from Vit 1 i   j   t-1
ctin Grady 2 from Vit 1 i   j-1 t-1

ctin Mcu  1 from Uphy 1 i   j   t-1
ctin Mcu  2 from Uphy 1 i-1 j   t-1
ctin Mcu  3 from Hphy 1 i   j   t-1
ctin Mcu  4 from Hphy 1 i+1 j   t-1
ctin Mcu  5 from Hphy 1 i-1 j   t-1

ctin Mcv  1 from Vphy 1 i   j+1 t-1
ctin Mcv  2 from Vphy 1 i   j   t-1
ctin Mcv  3 from Hphy 1 i   j   t-1
ctin Mcv  4 from Hphy 1 i   j+1 t-1
ctin Mcv  5 from Hphy 1 i   j-1 t-1

ctin Uphy 1 from Ufil  1 i   j   t-1
ctin Uphy 2 from Lamv  1 i   j   t
ctin Uphy 3 from Gradx 1 i   j   t
ctin Uphy 4 from Hz   1 i   j+1 t
ctin Uphy 5 from Hz   1 i   j   t
ctin Uphy 6 from Tau_forx 1 i j t
ctin Uphy 7 from Difu 1 i j t

ctin Vphy 1 from Vfil  1 i   j   t-1
ctin Vphy 2 from Lamu  1 i   j   t
ctin Vphy 3 from Grady 1 i   j   t
ctin Vphy 4 from Hz    1 i   j   t
ctin Vphy 5 from Hz    1 i-1 j   t
ctin Vphy 6 from Tau_fory 1 i j t
ctin Vphy 7 from Difv 1  i   j   t

ctin Hfil 1 from Hfil 1 i   j   t-1
ctin Hfil 2 from Hphy 1 i   j   t-1
ctin Hfil 3 from Hphy 1 i   j   t 

ctin Vfil 1 from Vfil 1 i   j   t-1
ctin Vfil 2 from Vphy 1 i   j   t-1
ctin Vfil 3 from Vphy 1 i   j   t 

ctin Ufil 1 from Ufil 1 i   j   t-1
ctin Ufil 2 from Uphy 1 i   j   t-1
ctin Ufil 3 from Uphy 1 i   j   t 

ctin Vit  1 from Uphy 1 i-1 j   t
ctin Vit  2 from Uphy 1 i   j   t
ctin Vit  3 from Vphy 1 i   j   t
ctin Vit  4 from Vphy 1 i   j+1 t
ctin Vit  5 from Hphy 1 i   j   t

ctin Vor  1 from Vphy 1 i+1 j   t
ctin Vor  2 from Vphy 1 i   j   t
ctin Vor  3 from Uphy 1 i   j   t
ctin Vor  4 from Uphy 1 i   j-1 t


order modinspace Soce
  order YB1 YA2
  	Mcu
	Mcv
        Hphy
	Hz 
   forder
   order YA1 YA2
   	Tau_forx
	Tau_fory
	Difu
	Difv
   	Lamu
	Lamv
	Gradx
	Grady
	Uphy 
	Vphy
	Hfil 
	Ufil 
	Vfil
  forder
  order YA1 YB2
   	 Vit
  forder
  order YB1 YA2
  	Vor
  forder
forder

order spaceintraj Toce
  Soce
forder

insert_fct arg xivg
insert_fct     xdisplay
insert_fct arg xgauss
insert_fct arg xwind
insert_fct arg xporte
insert_fct arg xcos
insert_fct     xvitgeo
insert_fct arg savegrad
insert_fct arg xsavestate
insert_fct arg xobs
insert_fct arg xsavenc
insert_fct arg read_lobs
insert_fct     compute_adjoint
insert_fct     renorm
insert_fct     set_sol
insert_fct arg xset_maxiter
insert_fct     load_allobs
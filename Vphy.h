forward(YREAL vfil, YREAL lamu, YREAL grady, YREAL hzp, YREAL hzm, YREAL tau_fory)
{
  if (Yt == 1) {
#ifdef GEOSTROPHY
    YS1 =  (grav / pcor) * (hzp - hzm)/dx;
#else
    YS1 = vfil ;
#endif

  }
  else {
    YREAL coef = 1 ;
    if(Yj == 0)
      YS1 = 0;
    else {
      if (Yi ==0)
	coef = 2 ; //to compensate the vorticity equal to zero
      YS1 = vfil + dedt*(-coef*lamu - grady/dy  -dissip*vfil + tau_fory);
    }
  }
}

backward(YREAL vfil, YREAL lamu, YREAL grady, YREAL hzp, YREAL hzm, YREAL tau_fory)
{
 if (Yt == 1) {
#ifdef GEOSTROPHY
    YJ1I4 =  (grav/pcor)/dx;
    YJ1I5 = -(grav/pcor)/dx;
#else
    YJ1I1 = 1 ;
#endif  
  }
 else {
   YREAL coef = 1;
   if(Yj>0)
     {
       if (Yi == 0)
	 coef = 2;
       YJ1I1 = 1-dedt*dissip ;
       YJ1I2 = -coef*dedt ;
       YJ1I3 = -dedt/dy ;
       YJ1I6 = dedt ;
     }  
 }
}

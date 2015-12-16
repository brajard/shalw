forward(YREAL vfil, YREAL lamu, YREAL grady, YREAL hzp, YREAL hzm, YREAL tau_fory, YREAL difv)
{
  if (Yt == 1) {
#ifdef GEOSTROPHY
    YS1 =  (grav / fcor[Yj]) * (hzp - hzm)/dx;
#else
    YS1 = vfil ;
#endif

  }
  else {
    YREAL coef = 1 ;
    if(Yj == 0 )
      YS1 = 0;
    else {
      if (Yi ==0)
	coef = 2 ; //to compensate the vorticity equal to zero
      YS1 = vfil + dedt*(-coef*lamu - grady/dy  -dissip*vfil + tau_fory + difv);
    }
  }
}

backward(YREAL vfil, YREAL lamu, YREAL grady, YREAL hzp, YREAL hzm, YREAL tau_fory, YREAL difv)
{
 if (Yt == 1) {
#ifdef GEOSTROPHY
    YJ1I4 =  (grav/fcor[Yj])/dx;
    YJ1I5 = -(grav/fcor[Yj])/dx;
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
       YJ1I7 = dedt ;
     }  
 }
}

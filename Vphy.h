forward(YREAL vfil, YREAL lamu, YREAL grady, YREAL hzp, YREAL hzm)
{
  if (Yt == 1) {
#ifdef GEOSTROPHY
    YS1 =  (grav / pcor) * (hzp - hzm)/dx;
#else
    YS1 = vfil ;
#endif

  }
  else
    if(Yj == 0)
      YS1 = 0;
    else 
      YS1 = vfil + dedt*(-lamu - grady/dy  -dissip*vfil);
}

backward(YREAL vfil, YREAL lamu, YREAL grady, YREAL hzp, YREAL hzm)
{
 if (Yt == 1) {
#ifdef GEOSTROPHY
    YJ1I4 =  (grav/pcor)/dx;
    YJ1I5 = -(grav/pcor)/dx;
#else
    YJ1I1 = 1 ;
#endif  
  }
  else
	if(Yj>0)
	{
	  YJ1I1 = 1-dedt*dissip ;
	  YJ1I2 = -dedt ;
	  YJ1I3 = dedt/dy ;
	}  
}

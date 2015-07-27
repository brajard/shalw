forward(YREAL a, YREAL b, YREAL c, YREAL d, YREAL e, YREAL f, YREAL g, YREAL hzp, YREAL hzm)
{
  if (Yt == 1) {
#ifdef GEOSTROPHY
    YS1 =  (grav / pcor) * (hzp - hzm)/dx;
#else
    YS1 = a ;
#endif

  }
  else
    if(Yj == 0)
      YS1 = 0;
    else YS1=a+dedt*((-grav/dy)*(b-c)-(pcor/4)*(d+e+f+g)-dissip*a);
}

backward(YREAL a, YREAL b, YREAL c, YREAL d, YREAL e, YREAL f, YREAL g, YREAL hzp, YREAL hzm)
{
 if (Yt == 1) {
#ifdef GEOSTROPHY
    // printf("(%d,%d) %f %f -----",Yi,Yj,hzp,hzm);
    //    YS1 =  - (grav / pcor) * (hzp - hzm)/dy;
    YJ1I8 =  (grav/pcor)/dx;
    YJ1I9 = -(grav/pcor)/dx;
#else
    YJ1I1 = 1 ;
#endif  
  }
  else
	if(Yj>0)
	{
	  YJ1I1 = 1-dedt*dissip;
	  YJ1I2 = -(dedt*grav)/dx;
	  YJ1I3 = -YJ1I2;
	  YJ1I4 = YJ1I5 = YJ1I6 = YJ1I7 = -(dedt*pcor)/4;
	}  
}

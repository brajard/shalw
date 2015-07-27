forward(YREAL a, YREAL b, YREAL c, YREAL d, YREAL e, YREAL f, YREAL g, YREAL hzp, YREAL hzm)
{
  //printf("Yi = %d, YA1 = %d", Yi, YA1);
  if (Yt == 1) {
#ifdef GEOSTROPHY
    // printf("(%d,%d) %f %f -----",Yi,Yj,hzp,hzm);
    YS1 =  - (grav / pcor) * (hzp - hzm)/dy;
#else
    YS1 = a ;
#endif
    
  }
  else
    if(Yi == YA1_Soce-1)
      YS1 = 0;
    else YS1 = a + dedt*((-grav/dx)*(b-c)+(pcor/4)*(d+e+f+g)-dissip*a);
  
}

backward(YREAL a, YREAL b, YREAL c, YREAL d, YREAL e, YREAL f, YREAL g, YREAL hzp, YREAL hzm)
{
//  printf("Yi = %d, YA1_Soce = %d", Yi, YTabMod[Yimod("Uphy")].axi);
  if (Yt == 1) {
#ifdef GEOSTROPHY
    // printf("(%d,%d) %f %f -----",Yi,Yj,hzp,hzm);
    //    YS1 =  - (grav / pcor) * (hzp - hzm)/dy;
    YJ1I8 = - (grav/pcor)/dy;
    YJ1I9 =   (grav/pcor)/dy;
#else
    YJ1I1 = 1 ;
#endif  
  }
  else
	if(Yi<YA1_Soce-1)
	{
	  YJ1I1 = 1 - dedt*dissip;
//	  printf("dedt=%f;dissip=%f\n",dedt,dissip);
			  YJ1I2 = - grav * (dedt/dx);
	  YJ1I3 = - YJ1I2;
	  YJ1I4 = YJ1I5 = YJ1I6 = YJ1I7 = (dedt/4)*pcor;
	}  
}

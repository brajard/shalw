forward(YREAL ufil, YREAL lamv, YREAL gradx, YREAL hzp, YREAL hzm)
{
  //printf("Yi = %d, YA1 = %d", Yi, YA1);
  if (Yt == 1) {
#ifdef GEOSTROPHY
    // printf("(%d,%d) %f %f -----",Yi,Yj,hzp,hzm);
    YS1 =  - (grav / pcor) * (hzp - hzm)/dy;
#else
    YS1 = ufil ;
#endif
    
  }
  else
    if(Yi == YA1_Soce-1)
      YS1 = 0;
    else 
      YS1 = ufil + dedt*(lamv - gradx/dx - dissip*ufil);
  
}

backward(YREAL ufil, YREAL lamv, YREAL gradx, YREAL hzp, YREAL hzm)
{
  if (Yt == 1) {
#ifdef GEOSTROPHY
    YJ1I4 = - (grav/pcor)/dy;
    YJ1I5 =   (grav/pcor)/dy;
#else
    YJ1I1 = 1 ;
#endif  
  }
  else
	if(Yi<YA1_Soce-1)
	{
	  YJ1I1 = 1 - dedt*dissip ;
	  YJ1I2 = dedt ;
	  YJ1I3 = -dedt/dx ;
	}  
}

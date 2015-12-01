forward(YREAL ufil, YREAL lamv, YREAL gradx, YREAL hzp, YREAL hzm, YREAL tau_forx, YREAL difu)
{
  //printf("Yi = %d, YA1 = %d", Yi, YA1);
  if (Yt == 1) {
#ifdef GEOSTROPHY
    // printf("(%d,%d) %f %f -----",Yi,Yj,hzp,hzm);
    YS1 =  - (grav / fcor[Yj]) * (hzp - hzm)/dy;
#else
    YS1 = ufil ;
#endif
    
  }
  else {
    YREAL coef = 1;
    if(Yi == YA1_Soce-1)
      YS1 = 0;
    else {
      if (Yj == YA2_Soce-1)
	coef = 2 ;
       if (Yi==98 && Yj==50)
      printf("difu(%d,%d,%d)=%g\n",Yi,Yj,YTemps,difu);
      YS1 = ufil + dedt*(coef*lamv - gradx/dx - dissip*ufil + tau_forx + difu);
    }
  }
}

backward(YREAL ufil, YREAL lamv, YREAL gradx, YREAL hzp, YREAL hzm, YREAL tau_forx, YREAL difu)
{
  if (Yt == 1) {
#ifdef GEOSTROPHY
    YJ1I4 = - (grav/fcor[Yj])/dy;
    YJ1I5 =   (grav/fcor[Yj])/dy;
#else
    YJ1I1 = 1 ;
#endif  
  }
  else {
    YREAL coef = 1;
    if(Yi<YA1_Soce-1)
      {
	if (Yj == YA2_Soce-1)
	  coef = 2;
	YJ1I1 = 1 - dedt*dissip ;
	YJ1I2 = coef*dedt ;
	YJ1I3 = -dedt/dx ;
	YJ1I6 = dedt ;
	YJ1I7 = dedt ;
      }  
  }
}

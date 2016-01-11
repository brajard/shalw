forward(YREAL ufil, YREAL lamv, YREAL gradx, YREAL tau_forx, YREAL difu, YREAL tau_fory_u, YREAL hep, YREAL hpp, YREAL hee, YREAL hpe, YREAL hem, YREAL hpm)
{
  //printf("Yi = %d, YA1 = %d", Yi, YA1);
  if (Yt == 1) {
#ifdef GEOSTROPHY
    // printf("(%d,%d) %f %f -----",Yi,Yj,hzp,hzm);
    //   YS1 =  - (grav / fcor[Yj]) * (hzp - hzm)/dy;
#else
    if (balanced == 1) {
      if (Yi == YA1_Soce-1) 
	YS1 = 0 ;
	else {
	  YREAL dh ;
	  YREAL c1,c2,c3 ;
	  c1 = (Yj == YA2_Soce-1) ? 0.5 : 0.25 ;
	  c2 = (Yj == 0) ? 0.5 : 0.25 ;
	  c3 = (Yj == YA2_Soce-1) ? 0.5 : 0.25 ;
	  dh = c1 * (hep + hpp + hee + hpe) - c2 * (hee + hpe + hem + hpm) ;
	  //la différence finie se fait sur un demi point de grille si Yj=0 ou YA2_Soce-1
	  if (Yj==0 || Yj==YA2_Soce-1)
	    dh = 2*dh;
	  YS1 = c3 * tau_fory_u/fcor[Yj] - (grav / fcor[Yj]) * dh / dy ;
	  
	}
      } // if balanced is true
	//      YS1 = tau_fory_u /fcor[Yj] - (grav / fcor[Yj]) * (hzp - hzm)/dy;
    else
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

backward(YREAL ufil, YREAL lamv, YREAL gradx, YREAL tau_forx, YREAL difu, YREAL tau_fory_u, YREAL hep, YREAL hpp, YREAL hee, YREAL hpe, YREAL hem, YREAL hpm)
{
  if (Yt == 1) {
#ifdef GEOSTROPHY
    YJ1I4 =  (grav/fcor[Yj])/dx;
    YJ1I5 = -(grav/fcor[Yj])/dx;
#else
    if (balanced == 1) {
      if (Yi!=YA1_Soce-1) {
	YREAL c1,c2,c3;
	c1 = (Yj == YA2_Soce-1) ? 0.5 : 0.25 ;
	c2 = (Yj == 0) ? 0.5 : 0.25 ;
	c3 = (Yj == YA2_Soce-1) ? 0.5 : 0.25 ;
	YJ1I6  = c3/fcor[Yj] ;
	YJ1I7  = -c1*(grav/fcor[Yj])/dy ;
	YJ1I8  = -c1*(grav/fcor[Yj])/dy ;
	YJ1I9  = -(c1-c2)*(grav/fcor[Yj])/dy ;
	YJ1I10 = -(c1-c2)*(grav/fcor[Yj])/dy ;
	YJ1I11 = c2*(grav/fcor[Yj])/dy ;
	YJ1I12 = c2*(grav/fcor[Yj])/dy ;
      }
    }
    else
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
	YJ1I4 = dedt ;
	YJ1I5 = dedt ;
      }  
  }
}

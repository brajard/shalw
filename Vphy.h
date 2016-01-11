forward(YREAL vfil, YREAL lamu, YREAL grady, YREAL tau_fory, YREAL difv, YREAL tau_forx_v, YREAL hee, YREAL hpe,YREAL hem,YREAL hpm,YREAL hme, YREAL hmm)
{
  if (Yt == 1) {

#ifdef GEOSTROPHY
//    YS1 =  (grav / fcor[Yj]) * (hzp - hzm)/dx;
#else
    if (balanced == 1) {
      if (Yj==0)
	YS1 = 0 ;
      else {
	YREAL dh;
	YREAL c1,c2,c3;
	c1 = (Yi == YA1_Soce-1) ? 0.5 : 0.25 ;
	c2 = (Yi == 0 ) ? 0.5 : 0.25 ;
	c3 = (Yi == 0 ) ? 0.5 : 0.25 ;
	dh = c1 * (hee + hpe + hem + hpm)  - c2 * (hme + hee + hmm + hem) ;
	//la différence finie se fait sur un demi point de grille si Yi=0 ou YA1_Soce-1
	if (Yi==0 || Yi==YA1_Soce-1)
	  dh = 2*dh;
	YS1 = - c3 * tau_forx_v/fcor[Yj] + (grav/fcor[Yj]) * dh / dx ;

      }
    }//if balanced is true
    else
      YS1 = vfil ;
  }
#endif
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

backward(YREAL vfil, YREAL lamu, YREAL grady, YREAL tau_fory, YREAL difv, YREAL tau_forx_v, YREAL hee, YREAL hpe,YREAL hem,YREAL hpm,YREAL hme, YREAL hmm)
{
 if (Yt == 1) {
#ifdef GEOSTROPHY
    YJ1I4 =  (grav/fcor[Yj])/dx;
    YJ1I5 = -(grav/fcor[Yj])/dx;
#else
    if (balanced == 1) {
      if (Yj!=0) {
	YREAL c1,c2,c3;
	c1 = (Yi == YA1_Soce-1) ? 0.5 : 0.25 ;
	c2 = (Yi == 0 ) ? 0.5 : 0.25 ;
	c3 = (Yi == 0 ) ? 0.5 : 0.25 ;
	YJ1I6 = -c3/fcor[Yj] ;
	YJ1I7 = (c1-c2)*(grav/fcor[Yj])/dx ;
	YJ1I8 = c1*(grav/fcor[Yj])/dx ;
	YJ1I9 = (c1-c2)*(grav/fcor[Yj])/dx ;
	YJ1I10 = c1*(grav/fcor[Yj])/dx ;
	YJ1I11 = -c2*(grav/fcor[Yj])/dx ;
	YJ1I12 = -c2*(grav/fcor[Yj])/dx ;

      }
    }
    else
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
       YJ1I4 = dedt ;
       YJ1I5 = dedt ;
     }  
 }
}

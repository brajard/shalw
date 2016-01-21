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
	YREAL dyh,dxh,Ax,Ay;
	YREAL c1,c2,c3;
	c1 = (Yi == YA1_Soce-1) ? 0.5 : 0.25 ;
	c2 = (Yi == 0 ) ? 0.5 : 0.25 ;
	c3 = (Yi == 0 ) ? 0.5 : 0.25 ;
	dxh = c1 * (hee + hpe + hem + hpm)  - c2 * (hme + hee + hmm + hem) ;
	dyh = hee- hem ;
	//la différence finie se fait sur un demi point de grille si Yi=0 ou YA1_Soce-1
	if (Yi==0 || Yi==YA1_Soce-1)
	  dxh = 2*dxh;
	Ax = grav * dxh/dx - c3 * tau_forx_v ;
	Ay = grav * dyh/dy - tau_fory ;
	YS1 = (fcor[Yj]*Ax - dissip*Ay)/(dissip*dissip + fcor[Yj]*fcor[Yj]) ;
	//	YS1 = - c3 * tau_forx_v/fcor[Yj] + (grav/fcor[Yj]) * dh / dx ;

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
	YREAL c1,c2,c3,coef;
	YREAL dydax,dyday ;
	YREAL dAydI4,dAydyh,dAxdI6,dAxdxh;
	YREAL dxhdI7, dxhdI8, dxhdI9, dxhdI10, dxhdI11, dxhdI12;
	YREAL dyhdI7, dyhdI9 ;
	YREAL denom = dissip*dissip + fcor[Yj]*fcor[Yj] ;

	dydax = fcor[Yj]/denom ;
	dyday = -dissip/denom ;

	coef = 1;
	c1 = (Yi == YA1_Soce-1) ? 0.5 : 0.25 ;
	c2 = (Yi == 0 ) ? 0.5 : 0.25 ;
	c3 = (Yi == 0 ) ? 0.5 : 0.25 ;
	if (Yi==0 || Yi==YA1_Soce-1)
	  coef = 2;

	dAxdI6 = -c3 ;
	dAxdxh = coef*grav/dx ;
	dAydI4 = -1 ;
	dAydyh = grav/dy ;

	dxhdI7 = c1 - c2 ;
	dxhdI8 = c1 ;
	dxhdI9 = c1 - c2 ;
	dxhdI10 = c1 ;
	dxhdI11 = - c2 ;
	dxhdI12 = - c2 ;

	dyhdI7 = 1 ;
	dyhdI9 = -1 ;

	
	YJ1I4 = dyday * dAydI4 ;
	YJ1I6 = dydax * dAxdI6 ;
	YJ1I7 = dydax * dAxdxh*dxhdI7 + dyday * dAydyh * dyhdI7;
	YJ1I8 = dydax * dAxdxh*dxhdI8 ;
	YJ1I9 = dydax * dAxdxh*dxhdI9  +  dyday * dAydyh * dyhdI9;
	YJ1I10 = dydax * dAxdxh*dxhdI10 ;
	YJ1I11 = dydax * dAxdxh*dxhdI11 ;
	YJ1I12 = dydax * dAxdxh*dxhdI12 ;

	/*	YJ1I6 = -c3/fcor[Yj] ;
	YJ1I7 = coef*(c1-c2)*(grav/fcor[Yj])/dx ;
	YJ1I8 = coef*c1*(grav/fcor[Yj])/dx ;
	YJ1I9 = coef*(c1-c2)*(grav/fcor[Yj])/dx ;
	YJ1I10 = coef*c1*(grav/fcor[Yj])/dx ;
	YJ1I11 = -coef*c2*(grav/fcor[Yj])/dx ;
	YJ1I12 = -coef*c2*(grav/fcor[Yj])/dx ;*/

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

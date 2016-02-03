forward(YREAL uip,YREAL uim, YREAL u, YREAL ujp, YREAL ujm) 
{
  YREAL Dx,Dy;
  Dx = (uip + uim - 2*u)/(dx*dx);
  Dy = (ujp + ujm - 2*u)/(dy*dy);
  YS1 = nu * (Dx + Dy) ;
}

backward(YREAL uip,YREAL uim, YREAL u, YREAL ujp, YREAL ujm)  
{
  YREAL idx2=nu/(dx*dx);
  YREAL idy2=nu/(dy*dy);

  YJ1I1 = idx2 ;
  YJ1I2 = idx2 ;
  YJ1I3 = -2* (idx2 + idy2) ;
  YJ1I4 = idy2 ;
  YJ1I5 = idy2 ;
}

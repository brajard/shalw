forward(YREAL taux, YREAL hm, YREAL hp) 
{
  #ifdef K_LIN
  YS1 = 0;
  #else
  YS1 = taux / (rho0 * (hmoy + 0.5*(hm+hp)));
  #endif
}

backward(YREAL tauxy, YREAL hm, YREAL hp) 
{
  #ifndef K_LIN
  YREAL denom = rho0 * (hmoy + 0.5*(hm+hp));
  YJ1I1 =  1 / denom ;
  YJ1I2 = YJ1I3 = - 0.5*rho0*taux / (denom*denom) ;
  #endif
  
}

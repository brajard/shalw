forward(YREAL taux, YREAL hm, YREAL hp) 
{
  #ifdef K_LIN
  YS1 = 0;
  #else
  YS1 = taux / (rho0 * (hmoy + 0.5*(hm+hp)));
  #endif
  // if (Yi==50 && Yj==50)
  //  printf("tau_forx(%d,%d,%d)=%g(%f,%f,%f)\n",Yi,Yj,YTemps,YS1,hmoy,rho0,taux);

}

backward(YREAL taux, YREAL hm, YREAL hp) 
{
  #ifndef K_LIN
  YREAL denom = rho0 * (hmoy + 0.5*(hm+hp));
  YJ1I1 =  1 / denom ;
  YJ1I2 = YJ1I3 = - 0.5*rho0*taux / (denom*denom) ;
  #endif
  
}

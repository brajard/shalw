forward(YREAL vp, YREAL vm, YREAL up, YREAL um) 
{
  YREAL ynlin = 0 ;
#ifndef K_LIN
  ynlin = (1.0/dx)*(vp-vm) - (1.0/dy)*(up-um) ;
#endif
  YS1 = ynlin + fcor[Yj] ;
}

backward(YREAL vp, YREAL vm, YREAL up, YREAL um) 
{
#ifndef K_LIN
  YJ1I1 = 1./dx ;
  YJ1I2 = -1./dx ;
  YJ1I3 = -1./dy ;
  YJ1I4 = 1./dy ;
#endif

}

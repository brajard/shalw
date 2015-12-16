forward(YREAL um, YREAL up, YREAL vm, YREAL vp, YREAL h) {
  YREAL ynlin = 0 ;
#ifndef K_LIN
  ynlin = 0.25 * ((um+up)*(um+up) + (vm+vp)*(vm+vp)) ;
#endif
  YS1 = ynlin + grav*h ;
}

backward(YREAL um, YREAL up, YREAL vm, YREAL vp, YREAL h) {
  YJ1I5 = grav ;
#ifndef K_LIN
  YJ1I1 = YJ1I2 = 0.5 * (um + up) ;
  YJ1I3 = YJ1I4 = 0.5 * (vm + vp) ;
#endif
}

  

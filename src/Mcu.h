forward(YREAL up, YREAL um, YREAL hc, YREAL hp, YREAL hm) 
{
  
  YREAL ylin,ynonlin=0;
  ylin = hmoy*(up - um);
  
#ifndef K_LIN
  ynonlin = 0.5*(up*(hp+hc) - um*(hm+hc));
#endif
  YS1 = ylin + ynonlin ;
}

backward(YREAL up, YREAL um, YREAL hc, YREAL hp, YREAL hm) 
{
  YREAL dylindup = hmoy ;
  YREAL dylindum = -hmoy ;
  YREAL dydup,dydum,dydhc,dydhp,dydhm;
  dydup=dydum=dydhc=dydhp=dydhm=0;
#ifndef K_LIN
  dydup = 0.5*(hp+hc);
  dydum = -0.5*(hm+hc);
  dydhc = 0.5*(up-um);
  dydhp = 0.5*up;
  dydhm = -0.5*um;
#endif
  YJ1I1 = dylindup + dydup ;
  YJ1I2 = dylindum + dydum ;
  YJ1I3 = dydhc ;
  YJ1I4 = dydhp;
  YJ1I5 = dydhm;
}

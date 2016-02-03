forward(YREAL Vorm, YREAL Vorp, YREAL u1, YREAL u2, YREAL u3, YREAL u4) 
{
  
  YS1 = 0.125 * (Vorm + Vorp) * (u1 + u2 + u3 + u4);

}

backward(YREAL Vorm, YREAL Vorp, YREAL u1, YREAL u2, YREAL u3, YREAL u4) 
{
  YJ1I1 = YJ1I2 = 0.125 * (u1 + u2 + u3 + u4);
  YJ1I3 = YJ1I4 = YJ1I5 = YJ1I6 = 0.125 * (Vorm + Vorp) ;
}

forward(YREAL hij, YREAL hipj, YREAL hijm, YREAL hipjm)
{
  if (Yt == 1)
    YS1 = 0.25*(hij + hipj + hijm + hipjm);
  else
    YS1 = 0;
}

backward(YREAL hij, YREAL hipj, YREAL hijm, YREAL hipjm)
{
  if (Yt == 1) {
    YJ1I1 = 0.25 ;
    YJ1I2 = 0.25 ;
    YJ1I3 = 0.25 ;
    YJ1I4 = 0.25 ;
  }
}

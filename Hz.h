forward(YREAL hij, YREAL hipj, YREAL hijm, YREAL hipjm)
{
  if (Yt == 1)
    YS1 = hij + hipj + hijm + hipjm;
  else
    YS1 = 0;
}

backward(YREAL hij, YREAL hipj, YREAL hijm, YREAL hipjm)
{
  if (Yt == 1) {
    YJ1I1 = 1 ;
    YJ1I2 = 1 ;
    YJ1I3 = 1 ;
    YJ1I4 = 1 ;
  }
}

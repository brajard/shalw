forward(YREAL hfil, YREAL mcu, YREAL mcv)
{
 

 if (YTemps==1) {
    YS1 = hfil;
  }
  else
    YS1 = hfil - dedt*(mcu/dx+mcv/dy);
}


backward(YREAL hfil, YREAL mcu, YREAL mcv)
{
  if (YTemps==1)
    YJ1I1 = 1;
  else {
    YJ1I1 = 1;
    YJ1I2 = -dedt/dx;
    YJ1I3 = -dedt/dy;
  }
}

forward(YREAL hfil, YREAL mcu, YREAL mcv)
{
	YS1 = hfil - dedt*(mcu/dx+mcv/dy);
}


backward(YREAL hfil, YREAL mcu, YREAL mcv)
{
	YJ1I1 = 1;
	YJ1I2 = -dedt/dx;
	YJ1I3 = -dedt/dy;
}

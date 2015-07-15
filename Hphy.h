forward(YREAL a, YREAL b, YREAL c, YREAL d, YREAL e)
{
//printf("(d-e)/dy = %f", (d-e)/dy );
//printf("(b-c)/dx = %f", (b-c)/dx );
//printf("a = %f  ", a );
	YS1 = a - dedt*hmoy*((b-c)/dx + (d-e)/dy);
//printf("YS1 = %f  ", YS1 );
}


backward(YREAL a, YREAL b, YREAL c, YREAL d, YREAL e)
{
	YJ1I1 = 1;
	YJ1I2 = - (dedt*hmoy)/dx;
	YJ1I3 = - YJ1I2;
	YJ1I4 = - (dedt*hmoy)/dy;
	YJ1I5 = - YJ1I4;
}

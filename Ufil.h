forward(YREAL a, YREAL b, YREAL c)
{
  if (Yt == 1)
    YS1 = c ;
  else
    if(Yt == 2)
      YS1 = a ;
    else YS1=b+alpha*(a-2*b+c);
}

backward(YREAL a, YREAL b, YREAL c)
{
  if (Yt == 1)
    YJ1I3 = 1;
  else
    if(Yt == 2)
      YJ1I1 = 1;
    else
      {
	YJ1I1 = alpha;
	YJ1I2 = 1 - 2*alpha;
	YJ1I3 = alpha;
      }  
}

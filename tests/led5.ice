algorithm blink(input int8 a,output int1 v)
{
  int32 w = 0;
  w = 0;
  v = 0;
loop:
  w = w + 1;
  v = ((w & (1 << a)) >> a);
  goto loop; 
}

algorithm main(output int8 led)
{
  blink b1;
  int20 v1    = 0;
  int8  myled = 0;
  
  led := myled;

  v1    = 24;
  myled = 0;
  
  b1 <- (v1);
  
loop:
  
  myled = 0;
  myled = myled | b1.v;
  myled = myled | (b1.v << 1);
  myled = myled | (b1.v << 2);
  myled = myled | (b1.v << 3);
  myled = myled | (b1.v << 4);
  myled = myled | (b1.v << 5);
  myled = myled | (b1.v << 6);
  myled = myled | (b1.v << 7);
  
  goto loop;
}

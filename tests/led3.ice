algorithm adder(input int8 a,input int8 b,output int8 v)
{
  v = a + b;
}

algorithm wait()
{
  int22 w = 0;
  w = 0;
loop:
  w = w + 1;
  if (w != 0) { goto loop; } else {}
}

algorithm main(output int8 led)
{
  wait  w;
  int8 myled = 0;
  
  led := myled;
  
  myled = 0;
  
loop:
  () <- w <- ();
  myled = 0;
  () <- w <- ();
  myled = 255;  
  goto loop;
}

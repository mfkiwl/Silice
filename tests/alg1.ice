algorithm adder(input int8 a,input int8 b,output int8 v)
{
  v = a + b;
}

algorithm main(output int8 led)
{

  int8 aa=0;
  int8 va1=0;

  adder a1(
    a <: aa,
    v :> va1
  );
  adder a2;
  
  int8 r=0;
  int8 a=1;
  int8 b=2;
  
  aa=11;
  a1.b=3 + va1;
  
  a1 <- ();  
  (a) <- a1;
  
  (a) <- a2 <- (r,b);  
}

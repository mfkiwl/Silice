algorithm foo(input uint8 i,output uint8 o)
{
  o = i;
}

algorithm main()
{
  foo   foo1;
  uint4 a = 0;
  int8  b = 12;
  
  foo   foo2(
    i <: a,
    o :> b
  );
  
  subroutine sub(input uint8 si,output uint8 so)
  {
    so = si;
  }
    
  {
    uint8 c = 0;
    c = a - b;
  }
  
  {
    uint8 d = 0;
    d = b >> 1;
  }

  {
    uint8 e = 0;
    e = 1 ? a : b;
  }

  {
    uint8 f = 0;
    f = {5{2b01}};
  }
  
  if (a < b) {
    a = 0;
  }
  
  foo1 <- (b);
  
  (a) <- foo1 <- (b);
  
  (a) <- sub <- (b);

}






















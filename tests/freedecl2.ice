algorithm bar(output uint8 v,input uint8 a)
{
  v = a;
}

algorithm main(output uint8 led)
{
  bar ibar1;
  
  subroutine foo(output uint4 v) 
  {
    uint4 d = 1;

    v = 3;

    e = 1;

    {
      uint4 e = 10;
      v = e + 1;
    }

  }

  uint8 a = 0;
  
  a = 2;
  b = 3;
  
  {
    uint8 c = 1;
    bar ibar2; // should trigger an error
    uint8 b = 1;
  
    a = b + 1;
  }
  
  (b) <- foo <- ();
  
}

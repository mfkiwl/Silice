algorithm main(output uint8 leds)
{
  uint8  i  = 0;
  uint8  e  = 0;
  uint8  pi = 0;
  uint8  a  = 255;
  uint8  b  = 255;
  uint8  c  = 255;
  uint64 o  = 0;

  a = 0;
  while (i < 8 + 3) { // the while will stop too early
  
    {
      pi = i;
      a  = a + 1;
    } -> {
			__display("A [%d] = %d (e = %d)",pi,b,e);
      b  = a + 10;
      e ^= pi;
    } -> {
      __display("B [%d] = %d (e = %d)",pi,b,e);
      c  = a + 2;
      __display("C [%d] = %d (e = %d)",pi,b,e);
    } -> {
      o[pi*8,8] = b;
    }

    i = i + 1;   
    
  }
 
  __display("%x",o);
  __display("e = %d",e);
 
}

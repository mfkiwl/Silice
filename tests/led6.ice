algorithm blink(input uint8 a,output uint1 v)
{
  uint8 w = 0;
loop:
  w = w + 1;
  v = ((w & (1 << a)) >> a);
  goto loop; 
}


algorithm main()
{
  uint8 led = 0;
  uint8 tmp = 0;
  
  blink b5;
  blink b6;
  blink b7;
  blink b8;
  
  uint8 a5 = 1;
  uint8 a6 = 2;
  uint8 a7 = 3;
  uint8 a8 = 4;
  uint8 myled = 0;
  
  led         := myled;

  b5 <- (a5);
  b6 <- (a6);
  b7 <- (a7);
  b8 <- (a8);
  
loop:
  
  myled = 0;
  myled = myled | (b5.v<<0);
  myled = myled | (b6.v<<1);
  myled = myled | (b7.v<<2);
  myled = myled | (b8.v<<3);
  myled = myled | (b8.v<<4);
  myled = myled | (b7.v<<5);
  myled = myled | (b6.v<<6);
  myled = myled | (b5.v<<7);

  $display("%b",myled);
  
  goto loop;
}

algorithm foo(input uint1 p)
{

  uint1 reg_p = 0;
  uint1 a = 0;
  
  reg_p := p;
  
  while (1) {
    a = reg_p;
    __display("%d",a);
  }

/*
  uint1 reg_p = 0;
  uint1 a = 0;
  
  reg_p ::= p;
  
  while (1) {
    a = reg_p;
    __display("%d",a);
  }
*/

/*
  uint1 reg1_p = 0;
  uint1 reg2_p = 0;
  uint1 a = 0;
  
  reg2_p := reg1_p;
  reg1_p := p;
  
  while (1) {
    a = reg2_p;
    __display("%d",a);
  }
*/

}
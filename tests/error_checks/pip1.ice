algorithm main(output int8 led)
{ 
  int8 a = 0;
  int8 b = 0;
  int8 c = 0;

  {
    a = 2;
  } -> {
    b = a;
    { 
      c = a;    
    } -> { 
      b = 3;
    } 
    
  }

}

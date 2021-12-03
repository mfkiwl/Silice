algorithm blink(output int1 b_fast,output int1 b_slow) <autorun>
{
  uint20 counter = 0;
  while (1) {
    counter = counter + 1;
    if (counter[0,8] == 0) {
      b_fast = !b_fast;
    }
    if (counter == 0) {
	  b_slow = !b_slow;
    }
  }	
}

algorithm main(output int8 led)
{
  int1 a = 0;
  int1 b = 0;
  blink b0(
    b_slow :> a,
    b_fast :> b
  );
  
  led      := 0; // turn off all eight LEDs to 
  led[0,1] := a; // first tracks slow blink
  led[1,1] := b; // second tracks fast blink

  while (1) { }  // inifinite loop, blink runs in parallel
}

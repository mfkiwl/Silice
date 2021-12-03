$$div_width=8
$$div_unsigned=1
$include('../../projects/common/divint_std.ice')

// Having no `main` algorithm somehow breaks the compiler...
algorithm main(output uint8 leds) {}


$$if FORMAL then

algorithm# x_div_x_eq_1(input uint$div_width$ x) <#depth=20, #mode=bmc & tind> {
  div$div_width$ div;
  uint$div_width$ result = uninitialized;

  #stableinput(x);
  #assume(x != 0);

  (result) <- div <- (x, x);

  #assert(result == 1);
}

$$end

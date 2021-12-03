group test {
  uint8  a = 0,
  uint9  b = 2,
  uint11 c = 4hf0
}

circuitry foo(
  inout t
) {
  t.c = t.a + 1;
  t.b = 23;
}

algorithm alg(
  output uint8 led,
  test t0 {
    input a,
    output b,
    output! c
  }
) {

  (t0) = foo(t0);

}

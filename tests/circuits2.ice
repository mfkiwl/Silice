circuitry test_circuit(input a,output b,output c)
{
  b = a + 1;
  c = b + 1;
}

algorithm main(output uint8 led)
{
  uint8 r = 1;
  while (1) {
    (r,r) = test_circuit(r); // combinational cycle through the circuitry
  }
}

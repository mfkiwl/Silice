
group grp {
  uint8 a = 0,
  uint8 b = 0,
}

algorithm Algo(grp v {output a,input b})
{
  uint32 cycle(0);

  always_after {
    cycle = cycle + 1;
  }

  v.a = v.b;
  __display("[cycle %d] (Algo) started",cycle);
}

algorithm main(output uint8 leds)
{
  Algo alg_inst;

  grp g;

  uint32 cycle(0);
  always_after {
    cycle = cycle + 1;
  }

  while (cycle < 15) {

    __display("[cycle %d] (main) calling",cycle);
    g.b = cycle;
    (g) <- alg_inst <- (g);
    __display("[cycle %d] (main) done, a = %d",cycle,g.a);
  }

}

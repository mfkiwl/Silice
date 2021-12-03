// Dual-port bram test

algorithm main(output uint8 led)
{
  dualport_bram uint8 mem[8] = {111,111,111,111,111,111,111,111};
  
  uint9 n    = 0;
  uint9 tmp1 = 0;
  uint8 tmp2 = 0;

  mem.wenable0 = 1;
  mem.wenable1 = 0;
  
  while (n < 10) { // we do two more iterations due to latencies
    // write at n
    if (n < 8) {
      mem.wdata0 = n;
      mem.addr0  = n;
    }    
    if (n > 0) {
      // read n-1
      mem.addr1  = n-1;
      if (n >= 2) {
        // display current, which was read at cycle (iteration) before: it is n-2
        tmp1 = n-2;
        tmp2 = mem.rdata1;
        __display("mem[%d] = %d",tmp1,tmp2);
      }
    }
    n = n + 1;
  }
  
}

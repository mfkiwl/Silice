
algorithm main(output  uint8   leds)
{
    uint16  instruction = uninitialized;
    uint1   dstackWrite := instruction[2,1];
    
    uint8 iter = 0;
    
    simple_dualport_bram uint16 dstack[256] = uninitialized;

    dstack.addr0    := 0;
    dstack.wenable1 := 1;

    while( iter < 16 ) {
        instruction = 0;
        if( dstackWrite ) {
            dstack.wdata1 = 0;
        }
        iter = iter + 1;
    }
    __write(".");
}

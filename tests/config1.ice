$$config['bram_wenable_width'] = '1'

algorithm main(
  output uint8 led 
)
{
  bram int8 test_ram[] = {1,2,3,4};
  brom int8 test_rom[] = {5,6,7,8};
  dualport_bram int8 test_dpram[] = {9,10,11,12};

  led = 0;
}


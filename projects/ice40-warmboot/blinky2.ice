// MIT license, see LICENSE_MIT in Silice repo root
// https://github.com/sylefeb/Silice
// @sylefeb 2021

import('../common/ice40_warmboot.v')

algorithm main(output uint5 leds,input uint3 btns)
{
  uint28 cnt(0);
  uint3  rbtns(0);
  uint1  pressed(0);

  uint1  boot(0);
  uint2  slot(0);
  ice40_warmboot wb(boot <: boot,slot <: slot);

  slot    := 2b00; // go to blinky1

  rbtns  ::= btns;  // register input buttons
  boot    := boot | (pressed & ~rbtns[0,1]); // set high on release
  pressed := rbtns[0,1]; // pressed tracks button 1

  leds    := cnt[23,1] ? 5b11110 : 5b0000;
  //                     ^^^^^^^ green LEDs blink
  cnt     := cnt + 1;

}

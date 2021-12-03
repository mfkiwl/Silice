// MIT license, see LICENSE_MIT in Silice repo root
// @sylefeb 2021
// https://github.com/sylefeb/Silice

#include "oled.h"

inline unsigned int time()
{
   unsigned int cycles;
   asm volatile ("rdcycle %0" : "=r"(cycles));
   return cycles>>1;
}

inline unsigned int core_id()
{
   unsigned int cycles;
   asm volatile ("rdcycle %0" : "=r"(cycles));
   return cycles&1;
}

void main()
{
  int s   = 0;
  int dir = 1;
  unsigned int cy_last = time();

  if (core_id() == 0) {

    while (1) {
      unsigned int cy = time();
      if (cy < cy_last) { cy_last = cy; } // counter may wrap around
      if (cy > cy_last + 1407) {
        *SOUND  = s;
        cy_last = cy;
        s += dir;
        if (s > 127 || s < -127) {
          dir = -dir;
        }
        *LEDS = cy;
      }
    }

  } else {

    int o = 0;
    oled_init();
    oled_fullscreen();
    while (1) {
      o+=4;
      for (int v=0;v<128;v++) {
        for (int u=0;u<128;u++) {
          oled_pix(u+o,v+o,0);
        }
      }
    }

  }

}

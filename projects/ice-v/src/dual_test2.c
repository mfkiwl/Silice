// MIT license, see LICENSE_MIT in Silice repo root
// @sylefeb 2021
// https://github.com/sylefeb/Silice

#include "config.h"

inline int core_id()
{
   unsigned int cycles;
   asm volatile ("rdcycle %0" : "=r"(cycles));
   return cycles;
}

void main()
{
  volatile int i = 0;

  *LEDS = 0x0f;

  int l = 1;

  if (core_id()&1) {
    while (1) {}
  }

  while (1) {

      l <<= 1;
      if (l > 8) {
        l = 1;
      }

      *LEDS = l;

      for (i=0;i<655350;i++) { }
      // for (i=0;i<7;i++) { }
  }

  //} else {
  //  while (1) {}
  //}
}

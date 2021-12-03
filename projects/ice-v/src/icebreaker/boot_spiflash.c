// MIT license, see LICENSE_MIT in Silice repo root
// https://github.com/sylefeb/Silice
// @sylefeb 2021

#include "../spiflash.c"
#include "../config.h"

static inline int core_id()
{
   unsigned int cycles;
   asm volatile ("rdcycle %0" : "=r"(cycles));
   return cycles&1;
}

volatile int synch = 0;

void main_load_code()
{
  *LEDS = 1;
  spiflash_init();
  *LEDS = 2;

  // copy to the start of the memory segment
  unsigned char *code = (unsigned char *)0x0000004;
  spiflash_copy(256000/*offset*/,code,4096/*SPRAM size*/);

  // jump!
  *LEDS = 7;
  synch = 1;
  asm volatile ("li t0,4; jalr x0,0(t0);");
}

void main_wait_n_jump()
{
  while (synch == 0) { }
  asm volatile ("li t0,4; jalr x0,0(t0);");
}

void main()
{

  if (core_id() == 0) {

    main_load_code();

  } else {

		main_wait_n_jump();

	}
}

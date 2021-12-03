/*
SL - 2021-02-03

SPIFLASH bit-banging from RV32I CPU

// MIT license, see LICENSE_MIT in Silice repo root

*/

#include "config.h"

static inline void spiflash_select()
{
  *SPIFLASH = 2;
}

static inline void spiflash_unselect()
{
  *SPIFLASH = 4 | 2;
}

#define spiflash_send_step(data) \
    mosi        = (data >> 7)&1;\
    data        = data << clk;\
    *SPIFLASH   = (mosi<<1) | clk;\
    clk         = 1-clk;\

void spiflash_send(int indata)
{
  register int clk  = 0;
  register int mosi = 0;
  register int data = indata;
  spiflash_send_step(data); spiflash_send_step(data);
  spiflash_send_step(data); spiflash_send_step(data);
  spiflash_send_step(data); spiflash_send_step(data);
  spiflash_send_step(data); spiflash_send_step(data);
  spiflash_send_step(data); spiflash_send_step(data);
  spiflash_send_step(data); spiflash_send_step(data);
  spiflash_send_step(data); spiflash_send_step(data);
  spiflash_send_step(data); spiflash_send_step(data);
}

#define spiflash_read_step_L() \
    *SPIFLASH    = 2;\
    asm volatile ("nop; nop; nop; nop; nop; nop;");\

#define spiflash_read_step_H() \
    *SPIFLASH    = 3;\
    n ++;\
    answer       = (answer << 1) | ((*SPIFLASH)&1);\

unsigned char spiflash_read()
{
    register int n = 0; 
    register int answer = 0xff;
    spiflash_read_step_L();
    while (n < 8) {
        spiflash_read_step_H();
        spiflash_read_step_L();
    }
    return answer & 255;
}

void spiflash_init()
{
  spiflash_select();
  spiflash_send(0xAB);
  spiflash_unselect();
}

unsigned char *spiflash_copy(int addr,unsigned char *dst,int len)
{
  spiflash_select();
  spiflash_send(0x03);
  spiflash_send((addr>>16)&255);
  spiflash_send((addr>> 8)&255);
  spiflash_send((addr    )&255);
  while (len > 0) {
    *dst  = spiflash_read();
    ++ dst;
    -- len;
  }
  spiflash_unselect();
  return dst;
}

void spiflash_read_begin(int addr)
{
  spiflash_select();
  spiflash_send(0x03);
  spiflash_send((addr>>16)&255);
  spiflash_send((addr>> 8)&255);
  spiflash_send((addr    )&255); 
}

unsigned char spiflash_read_next()
{
  return spiflash_read();
}

void spiflash_read_end()
{
  spiflash_unselect();
}

unsigned char spiflash_status()
{
  spiflash_select();
  spiflash_send(0x05);
  unsigned char status = spiflash_read();
  spiflash_unselect();
  return status;
}

void spiflash_busy_wait()
{
  while (spiflash_status() & 1) { }
}

#if 0

void spiflash_erase4KB(int addr)
{
  spiflash_select();
  spiflash_send(0x06); // enable write
  spiflash_unselect();
  
  spiflash_select();
  spiflash_send(0x20);
  spiflash_send((addr>>16)&255);
  spiflash_send((addr>> 8)&255);
  spiflash_send((addr    )&255); 
	spiflash_unselect();
}

void spiflash_write_begin(int addr)
{
  spiflash_select();
  spiflash_send(0x06); // enable write
  spiflash_unselect();

  spiflash_select();
  spiflash_send(0x02);
  spiflash_send((addr>>16)&255);
  spiflash_send((addr>> 8)&255);
  spiflash_send((addr    )&255);   
}

void spiflash_write_next(unsigned char v)
{
  spiflash_send(v);
}

void spiflash_write_end()
{
  spiflash_unselect();
}

#endif

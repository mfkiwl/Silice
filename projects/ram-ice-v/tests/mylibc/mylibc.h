#pragma once

extern volatile unsigned char* const FRAMEBUFFER;
extern volatile unsigned char* const AUDIO;
extern volatile unsigned char* const DATA;

typedef unsigned int size_t;

void   set_cursor(int x,int y);
int    putchar(int c);
int    puts(const char* s);
int    printf(const char *fmt,...);
void*  memcpy(void *dest, const void *src, size_t n); 
int    strcmp(const char *p1, const char *p2);
long   cpuid();
long   insn();
long   time();

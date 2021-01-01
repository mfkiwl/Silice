#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export PATH=$PATH:$DIR/../../tools/fpga-binutils/mingw32/bin/

if ! type "riscv32-unknown-elf-as" > /dev/null; then
  echo "defaulting to riscv64-linux"
  ARCH="riscv64-linux"
else
  ARCH="riscv32-unknown"
fi

GCC=`which riscv32-unknown-elf-gcc.exe`
RISCVLIB=`dirname $GCC`/../riscv32-unknown-elf/lib
echo "path to riscv libs $RISCVLIB"

echo "using $ARCH"

$ARCH-elf-gcc -w -DRISCV -DTIME -DUSE_MYSTDLIB -O2 -fno-pic -march=rv32i -mabi=ilp32 -c tests/mylibc/mylibc.c
$ARCH-elf-gcc -w -DRISCV -DTIME -DUSE_MYSTDLIB -O2 -fno-pic -march=rv32i -mabi=ilp32 -c tests/mylibc/mylibc_test.c

# $ARCH-elf-gcc -w -S -DRISCV -DTIME -DUSE_MYSTDLIB -O2 -fno-pic -march=rv32i -mabi=ilp32 -c tests/mylibc/mylibc.c

$ARCH-elf-as -march=rv32i -mabi=ilp32 -o div.o tests/mylibc/div.s
$ARCH-elf-as -march=rv32i -mabi=ilp32 -o crt0.o crt0.s

$ARCH-elf-ld -m elf32lriscv -b elf32-littleriscv -Tconfig_c.ld -o mylibc_test.elf div.o mylibc.o mylibc_test.o

$ARCH-elf-objcopy -O verilog mylibc_test.elf build/code0.hex

# uncomment to see the actual code, usefull for debugging
$ARCH-elf-objcopy.exe -O binary mylibc_test.elf build/code.bin
$ARCH-elf-objdump.exe -D -h -b binary -m riscv build/code.bin 

#!/bin/bash

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export PATH=$PATH:$DIR/../../tools/fpga-binutils/mingw32/bin/

source ../../tools/bash/find_riscv.sh

echo "using $ARCH"

BASE=./compile/icebreaker/swirl-cache
DST=./compile/build

$ARCH-gcc -DICEBREAKER_SWIRL_CACHE -fstack-reuse=none -fno-builtin -O3 -fno-stack-protector -fno-pic -march=rv32im -mabi=ilp32 -T$BASE/config_c.ld -ffreestanding -nostdlib -o $DST/code.elf $BASE/crt0.s $DIR/../../../src/printf.c $1

$ARCH-objcopy -O verilog $DST/code.elf $DST/code.hex

# output a binary blob of the code
$ARCH-objcopy.exe -O binary $DST/code.elf $DST/code.bin

# uncomment to see the actual code, useful for debugging
$ARCH-objdump.exe --disassemble $DST/code.elf > $DST/code.s

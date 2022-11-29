# Little Computer Architecture

Please note that the project is very early in the development process, so everything stated below can be changed over and over.

Little Computer is an emulated CPU designed to be used in educational institutions to better explain the path that it takes for a comuter program to be compiled, assembled and executed. This document describes the architecture and instruction set of the CPU as well as some of the I/O devices that it supports.

## Architecture

Little Computer is a 16-bit von Neumann CPU. All of the registers are 16 bits wide and the machine word is also 16 bits. The values are stored in little endian format (the least significant byte occupies the lower memory address than the most significant byte). The instructions can be anywhere from 1 to 3 bytes long.

## Registers

The CPU has 4 general purpose registers called R0 to R7.

Additional registers are:
* RSP - stack pointer, holds the address of the top of the stack
* RFL - flags register (read only for general purpose instructions, but individual bits can be changed as the result of executing an instruciton)
* RIP - instruction pointer registers, holds the address of the next instruction to be executed

## Memory

The CPU uses a flat memory model and peripheral devices can be mapped into the same memory address space as the RAM and ROM. The computer has 32 KiB ROM, 16 KiB of RAM and the remaining 16 KiB can be used for mapped I/O. The total address space therefore is 64 KiB.

| Address range   | Size   | Descriptio          |
|-----------------|--------|---------------------|
| 0x0000 - 0x7FFF | 32 KiB | ROM                 |
| 0x8000 - 0xBFFF | 16 KiB | RAM                 |
| 0xC000 - 0xFFFF | 16 KiB | I/O mappable memory |

The stack starts at 0x8000 and grows from low addresses to large addresses.

The CPU starts executing the program from address 0x7FF0.

## Clock frequency

The clock freqency is fixed at 1 MHz and all instructions are designed to take exactly one CPU cycle. It is not guaranteed that each single instruction will be executed in some fixed amount of time, so the application should not rely on counting instructions to perform time measuremes and synchronization. It is however guaranteed that one thousand CPU cycles will be executed every millisecond with a good precision.

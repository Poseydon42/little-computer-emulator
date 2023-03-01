# Little Computer Assembly

The Little Computer Assembly is a low-level language used to write programs for the Little Computer.

## Notations in this document

* Imm - immediate numeric value
* Mem - vaue stored in memory
* Rx - register X (R0...R7)
* [Imm] - value stored at the memory location pointed to by the immediate value
* [Rx] - value stored at the memory location pointed to by the value stored in the register Rx

Notations in the instruction encoding section:
* 0/1 - specifies the exact value that a specific bit should have
* x - the value of this bit is ignored
* HIGH(Imm) - the most significant byte of an immediate value
* LOW(Imm) - the least significant byte of an immediate value
* Rx1 - the first register operand or its binary encoding (depends on context)
* Rx2 - the second register operand or its binary encoding (depends on context)
* Op - bits of the opcode

## Instruction encoding

Each instruction is encoded by 1 - 4 bytes. An opcode is 6 bits long and is stored in the higher bits of the first byte of the instruction. Following it are zero to two 2-bit sequences that describe the types of the operands.

| Operand type     | Bit sequence |
|------------------|--------------|
| Register         | 0 0          |
| 1 byte immediate | 1 0          |
| 2 byte immediate | 1 1          |

The first byte of the instruction therefore holds the opcode and the type of the first operand (if applicable).

The second byte holds the second operand type (if applicable), the register index of the first operand (if applicable) and the register index of the second operand (if applicable).

The third and fourth bytes hold the low and high part of the immediate value argument (if applicable). An instruction can only have one argument that is an immediate value, and it is always stored in the third and fourth byte of an instruction even if an immediate value is the first operand.

In other words, this is how instructions with different types of arguments are encoded:

| Argument types                 | Byte 1 | Byte 2                      | Byte 3    | Byte 4    |
|--------------------------------|--------|-----------------------------|-----------|-----------|
| None                           | Op x x | n/a                         | n/a       | n/a       |
| Register                       | Op 0 0 | x x Rx1 Rx1 Rx1 x   x   x   | n/a       | n/a       |
| Register + Register            | Op 0 0 | 0 0 Rx1 Rx1 Rx1 Rx2 Rx2 Rx2 | n/a       | n/a       |
| Register + Immediate (1 byte)  | Op 0 0 | 1 0 Rx1 Rx1 Rx1 x   x   x   | Imm       | n/a       |
| Register + Immediate (2 bytes) | Op 0 0 | 1 1 Rx1 Rx1 Rx1 x   x   x   | LOW(Imm)  | HIGH(Imm) |
| Immediate (1 byte) + Register  | Op 1 0 | 0 0 x   x   x   Rx1 Rx1 Rx1 | Imm       | n/a       |
| Immediate (2 bytes) + Register | Op 1 1 | 0 0 x   x   x   Rx1 Rx1 Rx1 | LOW(Imm)  | HIGH(Imm) |

## Register indices

| Register | Index |
|----------|-------|
| R0       | 000   |
| R1       | 001   |
| R2       | 010   |
| R3       | 011   |
| RSP      | 100   |
| RFL      | 101   |

Register RIP (the instruction pointer register) does not have corresponding index as it cannot be changed directly, but rather as a side effect of executing some instructions.

## Instruction set

| Mnemonic               | Opcode | Description                                                                                                | Effect                                       |
|------------------------|--------|------------------------------------------------------------------------------------------------------------|----------------------------------------------|
| mov Rx, Imm/Rx         | 00001  | Moves data from a register or immediate value into another register                                        | Rx1 <- Imm/Rx2                               |
| lda Rx, Imm/Rx         | 00010  | Loads data from memory into a register                                                                     | Rx1 <- [Imm/Rx2]                             |
| sta Imm/Rx, Rx         | 00011  | Stores data from a register into memory                                                                    | [Imm/Rx1] <- Rx2                             |
| add Rx, Imm/Rx         | 00100  | Adds the second operand to the first operand and stores the result in the first operand                    | Rx1 <- Rx1 + Imm/Rx2                         |
| sub Rx, Imm/Rx         | 00101  | Subtracts the second operand from the first operand and stores the result in the first operand             | Rx1 <- Rx1 - Imm/Rx2                         |
| and Rx, Imm/Rx         | 00110  | Performs bitwise AND on the first and second operand and stores the result in the first operand            | Rx1 <- Rx1 AND Imm/Rx2                       |
| or Rx, Imm/Rx          | 00111  | Performs bitwise OR on the first and second operand and stores the result in the first operand             | Rx1 <- Rx1 OR Imm/Rx2                        |
| xor Rx, Imm/Rx         | 01000  | Performs bitwise XOR on the first and second operand and stores the result in the first operand            | Rx1 <- Rx1 XOR Imm/Rx2                       |
| not Rx                 | 01001  | Performs bitwise NOT on the operand and stores the result in the operand                                   | Rx1 <- NOT Rx1                               |
| shl Rx, Imm/Rx         | 01010  | Performs a left binary shift on the first operand by the offset specified in the second operand            | Rx1 <- Rx1 << Imm/Rx2                        |
| shr Rx, Imm/Rx         | 01011  | Performs a right binary shift on the first operand by the offset specified in the second operand           | Rx1 <- Rx2 >> Imm/Rx2                        |
| push Imm/Rx            | 01100  | Pushes the value of the operand onto the stack                                                             | [RSP] <- Imm/Rx1; RSP <- RSP + 2             |
| pop Rx                 | 01101  | Pops the value from the stack and stores it in the first operand                                           | Rx1 <- [RSP - 2]; RSP <- RSP - 2             |
| jmp Imm/Rx             | 01110  | Jumps to the address specified in the operand and continues execution from there                           | RIP <- Imm/Rx                                |
| jz Imm/Rx              | 01111  | Jumps to the address specified in the operand if the Z flag is set and continues execution from there      | if (Z) RIP <- Imm/Rx; endif                  |
| jv Imm/Rx              | 10000  | Jumps to the address specified in the operand if the V flag is set and continues execution from there      | if (V) RIP <- Imm/Rx; endif                  |
| jc Imm/Rx              | 10001  | Jumps to the address specified in the operand if the C flag is set and continues execution from there      | if (C) RIP <- Imm/Rx; endif                  |
| jn Imm/Rx              | 10010  | Jumps to the address specified in the operand if the N flag is set and continues execution from there      | if (N) RIP <- Imm/Rx; endif                  |
| call Imm/Rx            | 10011  | Pushes the address of the next instuction onto the stack and jumps to the address specified in the operand | [RSP] <- RIP; RSP <- RSP + 2; RIP <- Imm/Rx1 |
| ret                    | 10100  | Pops the value from the stack and jumps to the address specified by that value                             | RIP <- [RSP - 2]; RSP <- RSP - 2             |
| nop                    | 10101  | No operation (skip CPU cycle)                                                                              | -                                            |
| hlt                    | 00000  | Halts the CPU                                                                                              | Halt the CPU                                 |

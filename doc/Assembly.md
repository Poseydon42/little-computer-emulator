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

## Instruction encoding

Each instruction is encoded by 1 - 3 bytes. An opcode is encoded using 5 bits and a register index is 3 bits long, making it possible to encode simple instructions using only one byte. The register 

The first byte of the instruction specifies the opcode and the operand that is a register.

The second byte of the instruction specifies the low part of the immediate operand if one of the operands is an immediate value, or a register index of the second operand otherwise.

The third byte is only used if one of the operands is an immediate value and in such case it holds the high part of the immediate value.

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

| Mnemonic               | Description                                                                                                  |
|------------------------|--------------------------------------------------------------------------------------------------------------|
| mov Mem/Rx, Imm/Mem/Rx | Moves data from one location to another                                                                      |
| add Rx, Imm/Rx         | Adds the second operand to the first operand and stores the result in the first operand                      |
| sub Rx, Imm/Rx         | Subtracts the second operand from the first operand and stores the result in the first operand               |
| and Rx, Imm/Rx         | Performs bitwise AND on the first and second operand and stores the result in the first operand              |
| or Rx, Imm/Rx          | Performs bitwise OR on the first and second operand and stores the result in the first operand               |
| xor Rx, Imm/Rx         | Performs bitwise XOR on the first and second operand and stores the result in the first operand              |
| not Rx                 | Performs bitwise NOT on the operand and stores the result in the operand                                     |
| push Imm/Rx            | Pushes the value of the operand onto the stack                                                               |
| pop Rx                 | Pops the value from the stack and stores it in the first operand                                             |
| jmp Imm/Rx             | Jumps to the address specified in the operand and continues execution from there                             |
| jz Imm/Rx              | Jumps to the address specified in the operand if the ZERO flag is set and continues execution from there     |
| jg Imm/Rx              | Jumps to the address specified in the operand if the OVERFLOW flag is set and continues execution from there |
| call Imm/Rx            | Pushes the address of the next instuction onto the stack and jumps to the address specified in the operand   |
| ret                    | Pops the value from the stack and jumps to the address specified by that value                               |
| hlt                    | Halts the CPU                                                                                                |

## Instructions

### mov - Move data

| Instruction      | Effect       | Flags affected | Byte 1                | Byte 2                | Byte 3    |
|------------------|--------------|----------------|-----------------------|-----------------------|-----------|
| mov Rx1, Rx2     | Rx1 <- Rx2   |                | 0 0 0 0 1 Rx1 Rx1 Rx1 | Rx2 Rx2 Rx2 x x x x x |           |
| mov Rx1, Imm     | Rx1 <- Imm   |                | 0 0 0 1 0 Rx1 Rx1 Rx1 | LOW(Imm)              | HIGH(Imm) |
| mov Rx1, [Imm]   | Rx1 <- [Imm] |                | 0 0 0 1 1 Rx1 Rx1 Rx1 | LOW(Imm)              | HIGH(Imm) |
| mov Rx1, [Rx2]   | Rx1 <- [Rx2] |                | 0 0 1 0 0 Rx1 Rx1 Rx1 | Rx2 Rx2 Rx2 x x x x x |           |
| mov [Imm], Rx1   | [Imm] <- Rx1 |                | 0 0 1 0 1 Rx1 Rx1 Rx1 | LOW(Imm)              | HIGH(Imm) |
| mov [Rx1], [Rx2] | [Rx1] <- Rx2 |                | 0 0 1 1 0 Rx1 Rx1 Rx1 | Rx2 Rx2 Rx2 x x x x x |           |

### add - Add two values

| Instruction  | Effect           | Flags affected        | Byte 1                | Byte 2                | Byte 3    |
|--------------|------------------|-----------------------|-----------------------|-----------------------|-----------|
| add Rx1, Rx2 | Rx1 <- Rx1 + Rx2 | Carry, Overflow, Zero | 0 0 1 1 1 Rx1 Rx1 Rx1 | Rx2 Rx2 Rx2 x x x x x |           |
| add Rx1, Imm | Rx1 <- Rx1 + Imm | Carry, Overflow, Zero | 0 1 0 0 0 Rx1 Rx1 Rx1 | LOW(Imm)              | HIGH(Imm) |

### sub - Subtract two values

| Instruction  | Effect           | Flags affected        | Byte 1                | Byte 2                | Byte 3    |
|--------------|------------------|-----------------------|-----------------------|-----------------------|-----------|
| sub Rx1, Rx2 | Rx1 <- Rx1 - Rx2 | Carry, Overflow, Zero | 0 1 0 0 1 Rx1 Rx1 Rx1 | Rx2 Rx2 Rx2 x x x x x |           |
| sub Rx1, Imm | Rx1 <- Rx1 - Imm | Carry, Overflow, Zero | 0 1 0 1 0 Rx1 Rx1 Rx1 | LOW(Imm)              | HIGH(Imm) |

### and - Perform bitwise AND on two values

| Instruction  | Effect             | Flags affected | Byte 1                | Byte 2                | Byte 3    |
|--------------|--------------------|----------------|-----------------------|-----------------------|-----------|
| and Rx1, Rx2 | Rx1 <- Rx1 AND Rx2 | Zero           | 0 1 0 1 1 Rx1 Rx1 Rx1 | Rx2 Rx2 Rx2 x x x x x |           |
| and Rx1, Imm | Rx1 <- Rx1 AND Imm | Zero           | 0 1 1 0 0 Rx1 Rx1 Rx1 | LOW(Imm)              | HIGH(Imm) |

### or - Perform bitwise OR on two values

| Instruction  | Effect             | Flags affected | Byte 1                | Byte 2                | Byte 3    |
|--------------|--------------------|----------------|-----------------------|-----------------------|-----------|
| or Rx1, Rx2  | Rx1 <- Rx1 OR Rx2  | Zero           | 0 1 1 0 1 Rx1 Rx1 Rx1 | Rx2 Rx2 Rx2 x x x x x |           |
| or Rx1, Imm  | Rx1 <- Rx1 OR Imm  | Zero           | 0 1 1 1 0 Rx1 Rx1 Rx1 | LOW(Imm)              | HIGH(Imm) |

### xor - Perform bitwise XOR on two values

| Instruction  | Effect             | Flags affected | Byte 1                | Byte 2                | Byte 3    |
|--------------|--------------------|----------------|-----------------------|-----------------------|-----------|
| xor Rx1, Rx2 | Rx1 <- Rx1 XOR Rx2 | Zero           | 0 1 1 1 1 Rx1 Rx1 Rx1 | Rx2 Rx2 Rx2 x x x x x |           |
| xor Rx1, Imm | Rx1 <- Rx1 XOR Imm | Zero           | 1 0 0 0 0 Rx1 Rx1 Rx1 | LOW(Imm)              | HIGH(Imm) |

### not - Perform bitwise NOT on a value

| Instruction | Effect         | Flags affected | Byte 1                | Byte 2 | Byte 3 |
|-------------|----------------|----------------|-----------------------|--------|--------|
| not Rx1     | Rx1 <- NOT Rx1 | Zero           | 1 0 0 0 1 Rx1 Rx1 Rx1 |        |        |

### push - Push a value onto a stack

| Instruction | Effect                       | Flags affected | Byte 1                | Byte 2   | Byte 3    |
|-------------|------------------------------|----------------|-----------------------|----------|-----------|
| push Rx1    | [RSP] <- Rx1; RSP <- RSP + 2 |                | 1 0 0 1 0 Rx1 Rx1 Rx1 |          |           |
| push Imm    | [RSP] <- Imm; RSP <- RSP + 2 |                | 1 0 0 1 1 x x x       | LOW(Imm) | HIGH(Imm) |

### pop - Pop the value off the stack

| Instruction | Effect   | Flags affected        | Byte 1                | Byte 2 | Byte 3 |
|-------------|----------|-----------------------|-----------------------|--------|--------|
| pop Rx1     | Rx1 <- [RSP - 2]; RSP <- RSP - 2 | 1 0 1 0 0 Rx1 Rx1 Rx1 |        |        |

### jmp - Unconditional jump to an address

| Instruction | Effect     | Flags affected | Byte 1                | Byte 2   | Byte 3    |
|-------------|------------|----------------|-----------------------|----------|-----------|
| jmp Imm     | RIP <- Imm |                | 1 0 1 0 1 x x x       | LOW(Imm) | HIGH(Imm) |
| jmp Rx1     | RIP <- Rx1 |                | 1 0 1 1 0 Rx1 Rx1 Rx1 |          |           |

### jz - Jump if zero flag is set

| Instruction | Effect                         | Flags affected | Byte 1                | Byte 2   | Byte 3    |
|-------------|--------------------------------|----------------|-----------------------|----------|-----------|
| jz Imm      | if (zero is set) => RIP <- Imm |                | 1 0 1 1 1 x x x       | LOW(Imm) | HIGH(Imm) |
| jz Rx1      | if (zero is set) => RIP <- Rx1 |                | 1 1 0 0 0 Rx1 Rx1 Rx1 |          |           |

### jg - Jump if overflow flag is set

| Instruction | Effect                             | Flags affected | Byte 1                | Byte 2   | Byte 3    |
|-------------|------------------------------------|----------------|-----------------------|----------|-----------|
| jz Imm      | if (overflow is set) => RIP <- Imm |                | 1 1 0 0 1 x x x       | LOW(Imm) | HIGH(Imm) |
| jz Rx1      | if (overflow is set) => RIP <- Rx1 |                | 1 1 0 1 0 Rx1 Rx1 Rx1 |          |           |

### call - Call a function

| Instruction | Effect                                   | Flags affected | Byte 1                | Byte 2   | Byte 3    |
|-------------|------------------------------------------|----------------|-----------------------|----------|-----------|
| call Imm    | [RSP] <- RIP; RSP <- RSP + 2; RIP <- Imm |                | 1 1 0 1 1 x x x       | LOW(Imm) | HIGH(Imm) |
| call Rx1    | [RSP] <- RIP; RSP <- RSP + 2; RIP <- Rx1 |                | 1 1 1 0 0 Rx1 Rx1 Rx1 |          |           |

### ret - Return from a function

| Instruction | Effect                           | Flags affected | Byte 1          | Byte 2   | Byte 3    |
|-------------|----------------------------------|----------------|-----------------|----------|-----------|
| ret         | RIP <- [RSP - 2]; RSP <- RSP - 2 |                | 1 1 1 0 1 x x x |          |           |

### hlt - Halt the CPU

| Instruction | Effect        | Flags affected | Byte 1          | Byte 2   | Byte 3    |
|-------------|---------------|----------------|-----------------|----------|-----------|
| ret         | CPU is halted |                | 0 0 0 0 0 0 0 0 |          |           |

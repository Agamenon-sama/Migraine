# Technical Sheet

## Mnemonics

**00E0** - *cls* : clear screen

**00EE** - *ret* : return from function

**1NNN** - *jmp  addr* : jumps to location **nnn**

**2NNN** - *call addr* : call functions by jumping to location **nnn** and incrementing stack pointer with program counter

**3XNN** - *se   vx byte* : skip next instruction if **vx** == **nn**

**4XNN** - *sne  vx byte* : skip next instruction if **vx** != **nn**

**5XY0** - *sev  vx vy* : skip next instruction if **vx** == **vy**

**6XNN** - *ld   vx byte* : loads value **nn** into register **vx**

**7XNN** - *addv vx byte* : adds the value **byte** to the value stored in **vx**

**8XY0** - *ldv  vx vy* : loads value of **vy** in **vx**

**8XY1** - *or   vx vy* : performs bitwise OR on the values of **vx** and **vy** and stores the result in **vx**

**8XY2** - *and  vx vy* : performs bitwise AND on the values of **vx** and **vy** and stores the result in **vx**

**8XY3** - *xor  vx vy* : performs bitwise XOR on the values of **vx** and **vy** and stores the result in **vx**

**8XY4** - *add  vx vy* : adds the value of **vy** to **vx**

**8XY5** - *sub  vx vy* : substracts the values of **vy** from **vx** and set **vf** if no borrow

**8XY6** - *shr  vx vy* : shift right the value of **vx** and store the discarded bit in **vf**

**8XY7** - *subn vx vy* : substracts the values of **vx** from **vy** and set **vf** if no borrow

**8XYE** - *shl  vx vy* : shift left the value of **vx** and store the discarded bit in **vf**

**9XY0** - *sne  vx vy* : skip next instruction if **vx** != **vy**

**ANNN** - *ldi  addr* : set register **i** to **nnn**

**BNNN** - *jmpv addr* : program counter **pc** is set to **nnn** + **v0**

**CXNN** - *rnd  vx byte* : generates random number, AND it with the given byte **nn** and store it in **vx**

**DXYN** - *drw  vx vy nibble* : draw **n** bytes from memory starting from **i** to the screen at position (**vx**, **vy**)

**EX9E** - *kpr  vx* : skip next instruction if key value in **vx** is pressed

**EXA1** - *knpr vx* : skip next instruction if key value in **vx** is not pressed

**FX07** - *gdt  vx* : store (get) value of delay timer in **vx**

**FX0A** - *gkpr vx* : store (get) value of key press in **vx**

**FX15** - *sdt  vx* : set delay timer to **vx**

**FX18** - *sst  vx* : set sound timer to **vx**

**FX1E** - *addi vx* : add **vx** to **i**

**FX29** - *lds  vx* : set i to the location of the predefined sprite determined by setting **vx**

**FX33** - *bcd  vx* : store bcd representation of **vx** in **i**, **i**+1, **i**+2

**FX55** - *regd vx* : store (dump) values in registers from **v0** to **vx** in memory starting from location **i**

**FX65** - *regl vx* : fills (load) values to registers from **v0** to **vx** from memory starting from location **i**


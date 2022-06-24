    lw 0 1 six  load 6 to reg1
    jalr 1 2    store 2 to reg2, branch to 6
L1  add 1 2 3   add %1 + %2 -> %3
    lw 0 4 two  load 2 to reg4
    add 4 1 4   add %4 + %1 -> %4
    beq 0 0 1   if reg3==reg4, go to end
    beq 0 0 L1  branch to L1
    noop
done halt
two .fill 2
six .fill 6

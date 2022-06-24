start   lw 0 1 one  reg1 = 1
    lw 0 2 two      reg2 = 2
    noop            3stall
    noop
    noop
    add 1 1 3       reg1 + reg1 = reg3
    beq 0 1 start   no jump
    sw 0 1 11       mem[11] = 1
    sw 0 2 10       mem[10] = 2
    halt
one .fill 1
two .fill 2

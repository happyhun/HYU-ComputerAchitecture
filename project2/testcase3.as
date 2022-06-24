    lw 0 1 six  load 6 to reg1
    lw 0 2 two  load 2 to reg4
    noop        3stall
    noop
    noop
    add 1 2 3   add %1 + %2 -> %3
    nor 1 2 4   nor reg1, reg2 -> reg4
    halt
two .fill 2
six .fill 6

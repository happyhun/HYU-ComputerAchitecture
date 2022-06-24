    lw 0 1 one  load 1 to reg1
    lw 0 2 two  load 2 to reg2
    lw 0 3 nine load 9 to reg3
    noop        reg1, reg2 load-use data hazards
    noop        after 3cycles, we can use updated reg1, reg2
    add 1 2 1   add %1 + %2 -> %1
    sw 0 3 9   store 9 to mem[9]
    halt
one .fill 1
two .fill 2
nine .fill 9

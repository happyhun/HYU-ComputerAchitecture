    lw 0 1 three load 3 to reg1
    lw 0 2 four  load 4 to reg2
    add 1 2 3    add %1 + %2 -> %3
    nor 1 3 4    nor %1 + %3 -> %4
    nor 2 3 5    nor %2 + %3 -> %5
    add 4 5 5    add %4 + %5 -> %5
    sw 2 5 6     store -16 to mem[10]
done halt
three .fill 3
four .fill 4
ans .fill 0

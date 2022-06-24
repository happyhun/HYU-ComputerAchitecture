    lw 0 1 seven    load 7 to reg1
    lw 0 2 neg1     load -1 to reg2
    add 1 2 3       add %1 + %2 -> %3
    nor 1 3 1       nor %1, %3 -> %1
    add 3 1 3       add %3 + %1 -> %3
    add 2 2 2       add %2 + %2 -> %2
    beq 2 3 1       if reg2==reg3, go to noop
    sw 0 0 ans      if reg2!=reg3, ans=0
    noop
done halt
seven .fill 7
neg1 .fill -1
ans .fill 1

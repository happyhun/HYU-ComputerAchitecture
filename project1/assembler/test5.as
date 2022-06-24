    lw 0 1 one
    lw 0 2 ten
start   add 1 3 3
    beq 3 2 1
    beq 0 0 start
    add 2 3 4
done halt
one .fill 1
ten .fill 10

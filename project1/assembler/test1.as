    lw 0 1 one  load 1 to reg1
    lw 0 2 two  load 2 to reg2
    lw 2 3 8    load 9 to reg3
start   add 1 2 1   add %1 + %2 -> %1
    beq 3 1 2   goto end of program when reg1==9
    beq 0 0 start go back to the beginning of the loop
    noop
done halt
one .fill 1
two .fill 2
nine .fill 9

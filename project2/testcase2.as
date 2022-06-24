    lw 0 1 data1
    lw 0 2 data2
    beq 0 0 label   branch hazard, 2stall
    noop
    noop
    nor 1 2 3
label   add 1 2 3
    halt
data1   .fill 10
data2   .fill 20

    lw 0 1 seven    load 7 to reg1
    lw 0 2 neg1     load -1 to reg2
    lw 0 3 neg1     load -1 to reg3
    lw 0 4 neg1     load -1 to reg4
    add 0 1 5       add reg0 + reg1 -> reg5
    add 1 2 3       add reg1 + reg2 -> reg3
    halt
seven .fill 7
neg1 .fill -1

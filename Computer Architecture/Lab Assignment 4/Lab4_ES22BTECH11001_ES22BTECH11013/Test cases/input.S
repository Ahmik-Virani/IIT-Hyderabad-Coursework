addi x5, x5, 0
lui x4, 0x10
addi x5, x5, -1
sd x5, 2(x4)
ld x10, 2(x4)
lb x11, 2(x4)
lbu x12, 2(x4)
.data
.dword -5322, -2443, -999222, -9855, -3344, -32111, -999, -03827
.text
lui x3, 0x10
ld x10, 0(x3)
lw x12, 8(x3)
lh x14 16(x3)
lb x13, 24(x3)
lwu x16, 32(x3)
lhu x15, 40(x3)
lbu x11, 48(x3)
addi s1, a0, -2048
andi a1, x12, 2047
ori x13, x14, 1000
xori a2, a3, -1000
slli x15, x16, 5
srli a4, a5, 5
srai x17, a6, 5
ld x1, 2047(x18)
lw x19, 248(x20)
lh s2, 1000(s3)
lb x21, 100(x22)
lwu s4, 2047(s5)
lhu x23, 248(x24)
lbu s6, 0(s7)
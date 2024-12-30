.data
.dword -5322, -2443
.text
lui x3, 0x10
ld x1, 0(x3)
ld x2, 8(x3)
sra x4, x1, x2
srl x5, x1, x2
sll x6, x1, x2
srai x7, x1, 63
srli x8, x1, 63
slli x9, x1, 63
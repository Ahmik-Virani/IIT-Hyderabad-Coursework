.data
.dword 12345678987654321, 50
.word 1234232, -362, -362
.half 48363, -362
.byte 4, 36
.text
lui x4, 0x10
ld x2, 0(x4)
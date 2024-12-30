.data
.dword 1232335, 3000, 976855, 32111
.text
lui x3, 0x10
ld x24, 0(x3)
ld x27, 8(x3)
ld x26, 16(x3)
ld x29, 24(x3)
sw s8, 248(s9)
sh x27, 1000(x28)
sb s10, 0(s11)
sd x29, 0(x30)
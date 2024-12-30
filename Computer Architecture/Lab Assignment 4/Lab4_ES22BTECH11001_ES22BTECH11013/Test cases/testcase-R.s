.data
.dword 12345, 23443, 222332, 976855, 3344, 32111, 92838, 03827
.text
lui x3, 0x10
ld x2, 0(x3)
ld x4, 8(x3)
ld x5 16(x3)
ld x7, 24(x3)
ld x8, 32(x3)
ld x10, 40(x3)
ld x11, 48(x3)
add x1, x0, x2
sub ra, zero, sp 
and x4, x4, x5
or tp, tp, t0
xor x6, x7, x8 
sll t1, t2, s0 
srl t1, t2, fp 
sra x9, x10, x11

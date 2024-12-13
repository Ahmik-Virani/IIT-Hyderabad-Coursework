#Assignment by:
#ES22BTECH11001: Ahmik Virani
#ES22BTECH11013: Divya Rajparia

.section .data             # (instead of .data in Ripes)
L1: .word 10000000 #delay count to be loaded from memory

.section .text
.global main   # (add this to indicate main is a global function, need not be there in Ripes)
main:
la x20, L1          #this will load the ADDRESS of the data section in x3

#YOUR CODE FOLLOWS HERE. The ADDRESS of the data segment is available in x3
li x1, 0x00000000
li x2, 0x10012004
sw x1, 0(x2)

li x3, 0x00000020
li x4, 0x10012008
sw x3, 0(x4)

li x5, 0x1001200C

addi x13, x0, 0
addi x12, x0, 5

LOOP:

sw x3, 0(x5)

addi x10, x0, 0
addi x11, x0, 0
lw x11, 0(x20)
DELAY_1:
    addi x10, x10, 1
    blt x10, x11, DELAY_1


sw x1, 0(x5)

addi x10, x0, 0
DELAY_2:
    addi x10, x10, 1
    blt x10, x11, DELAY_2

addi x13, x13, 1
blt x13, x12, LOOP


#At the end, have a while(1) loop, as shown below
Lwhile1: j Lwhile1

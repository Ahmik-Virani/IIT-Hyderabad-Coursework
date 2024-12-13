#Ahmik Virani
#ES22BTECH11001

.data

.dword 1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 523, 524, 525, 533, 512

.text
    lui x3, 0x10000

    #Initializing the value of x10 as the first value to be added
    ld x10, 0(x3)
    
    #Using x9 as dummy register to store the values to be added/subtracted
    ld x9, 8(x3)
    add x10, x9, x10
    
    ld x9, 16(x3)
    add x10, x9, x10
    
    ld x9, 24(x3)
    add x10, x9, x10
    
    ld x9, 32(x3)
    add x10, x9, x10
    
    ld x9, 40(x3)
    add x10, x9, x10
    
    ld x9, 48(x3)
    add x10, x9, x10
    
    ld x9, 56(x3)
    add x10, x9, x10
    
    ld x9, 64(x3)
    add x10, x9, x10
    
    ld x9, 72(x3)
    add x10, x9, x10
    
    ld x9, 80(x3)
    sub x10, x10, x9
    
    ld x9, 88(x3)
    sub x10, x10, x9
    
    ld x9, 96(x3)
    sub x10, x10, x9
    
    ld x9, 104(x3)
    sub x10, x10, x9
    
    ld x9, 112(x3)
    sub x10, x10, x9
    
    #the final value is stored in x10
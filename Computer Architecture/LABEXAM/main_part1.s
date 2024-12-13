.data
.dword 3, 3, 3, 3, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1 
.text

main:
    lui x3, 0x10000
    ld x10, 0(x3)    #rowA
    ld x11, 8(x3)    #colA
    ld x12, 16(x3)   #rowB
    ld x13, 24(x3)   #colB
    
    addi x14, x3, 32   #Base address of matrix A
    mul x31, x10, x11 #Number of elements in A
    slli x31, x31, 3   # Note x31 is being used as a dummy register, no need to save it
    add x15, x14, x31   #Base address of matrix B
    
    li x16, 0x10004000    #Base address for storing x16
    
    jal x1, mmult
    beq x0, x0, EXIT
    
mmult:
    
    # Error handling
    # If any number of rows, or columns are 0, then we go to error
    beq x10, x0, REPORT_ERROR
    beq x11, x0, REPORT_ERROR
    beq x12, x0, REPORT_ERROR
    beq x13, x0, REPORT_ERROR
    
    # If columns of A != rows of B, error
    bne x11, x12, REPORT_ERROR
    
    # Now, just store the number of rows and columns in C
    sd x10, 0(x16)
    sd x13, 8(x16)
    
    addi x16, x16, 16
    
    #Now x16 stores the base address of matrix C
    
    li x18, 0   # row counter for A
    li x19, 0   # column counter for A
    li x20, 0   # row counter for B
    li x21, 0   # column counter for B
    
    #Below i am running the loops as it is from the code given in the question
    
    LOOP1:
        beq x18, x10, DONE    #IF all rows done, DONE
        addi x21, x0, 0
        LOOP2:
            #x9 has curent address of C
            
            mul x9, x18, x13
            add x9, x9, x21
            slli x9, x9, 3
            add x9, x9, x16
            
            sd x0, 0(x9)
            
            addi x19, x0, 0
            LOOP3:
                
                # Calculate the current index, since we are going in row major order it equals row_index * no_of_columns + col_index
                mul x30, x18, x11
                add x30, x30, x19
                slli x30, x30, 3
                
                mul x31, x19, x13
                add x31, x31, x21
                slli x31, x31, 3
                
                add x25, x30, x14    # Current address of A[i][k]
                add x26, x31, x15    # Current address of B[k][j]
                
                ld x30, 0(x25)        # Value of A[i][k]
                ld x31, 0(x26)        # Value of B[k][j]
                
                # Multiply and add in the sum
                mul x27, x30, x31
                
                ld x29, 0(x9)
                add x29, x29, x27
                sd x29, 0(x9)
                
                addi x19, x19, 1
                bne x19, x12, LOOP3
            addi x21, x21, 1
            bne x21, x13, LOOP2
        addi x18, x18, 1
        beq x0, x0, LOOP1
        
    DONE:
        # Returning void
        jalr x0, x1(0)
        
        
REPORT_ERROR:
    sd x0, 0(x16)
    sd x0, 8(x16)

EXIT:
    # Exit the program
    addi x0, x0, 0
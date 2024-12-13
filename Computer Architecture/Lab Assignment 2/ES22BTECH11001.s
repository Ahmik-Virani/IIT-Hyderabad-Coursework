#Ahmik Virani
#ES22BTECH11001

.data
.dword 4 10 5 93 31 4 5 3 0

.text 
    lui x2, 0x10000        #Load the first double word address in x2 register
    lui x20, 0x10000       #This register is to store the address where the output should be displayed
    addi x20, x20 0x200    #Since it is 8 bits, we need to do lui first then add rest of bits
    
    ld x3, 0(x2)     #x3 is the counter
    addi x2, x2, 8   #We start from the double word after the counter
    
    beq x3, x0, DONE        #Handling the edge case when no input is given
    

LOOP: 
    
    ld x10, 0(x2)        #First we load the first word
    ld x11, 8(x2)        #Then we load the second word
    
    beq x11, x0, EXIT    #If value at x11 is zero, we need to output 0
    blt x0, x10, FOR     #If value at x10 is zero, we need to print zero, otherwise we branch to the FOR loop
    add x11, x0, x0      #If value at x10 is 0, We make x11 zero and then save the value of x11
    beq x0, x0, EXIT     #Unconditional jump is x10 is 0
    
FOR: 
    blt x11, x10, SWAP   #Ensure that x10 is less than x11 (We are doing euclidian algorithm)
    beq x10, x0, EXIT    #If value at x10 is zero, we store the state of x11
    sub x11, x11, x10    #Otherwise we subtract x10 from x11
    beq x0, x0, FOR      #Then we repeat the above 3 steps of for loop
    

SWAP: 
    add x13, x11, x0     #We store the value of x11 in x13
    add x11, x10, x0     #We keep the value of x10 in x11
    add x10, x13, x0     #We keep the stored value of x11 in x10
    beq x0, x0 FOR       #We go back to the FOR label
      
EXIT:
    sd x11, 0(x20)       #We store the value of register x11 at the required location
    addi x3, x3, -1      #We decrement counter by 1
    beq x3, x0, DONE     #If counter is 0 then we are done finding GCD of the numbers
    addi x20, x20, 8     #Otherwise we store the address than the next answer should be stored
    addi x2, x2, 16      #We store the address that the next pair of words start from
    beq x0, x0, LOOP     #Then we go back to the label LOOP and compute GCD for the next pair of words

DONE:
    addi x0, x0, 0       #We are done
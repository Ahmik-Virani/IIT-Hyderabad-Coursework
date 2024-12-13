.data
.dword 11, 52 
.dword 1
.dword 0x0010004001203120, 0x0010004001203120

# My code starts here, please edit the test cases above this comment line

error:  .asciz "Error! bits for fraction + mantissa > 63" # string with null terminator

.text
main:
    lui x3, 0x10000
    ld x28, 0(x3)        # x28 stores the number of bits for exponent
    ld x29, 8(x3)        # x29 stores the number of bits for mantissa
    
    add x31, x28, x29
    addi x30, x0, 64
    bge x31, x30, report_error
    
    
    addi x15, x3, 0x200  # This is destination address
    
    addi sp, sp, -16
    sd x29, 16(sp)        # Store these values so they can be used across functions
    sd x28, 8(sp)
    
    # I am using x17 as a dummy register for calculations
    addi x17, x0, 1
    sll x12, x17, x29
    addi x12, x12, -1    # Mask for mantissa
    
    addi x13, x12, 1
    sll x13, x13, x28
    addi x13, x13, -1
    xor x13, x13, x12    # Mask for exponent
    
    addi x17, x0, 1
    sll x14, x17, x28
    srli x14, x14, 1
    addi x14, x14, -1    # This is the bias
   
    # Now lets begin the loop which calls the functions for the pair of numbers which are provided 
    addi x3, x3, 0x10
    ld x5, 0(x3)         # x5 stores the number of times we need to add and multiply
    addi x3, x3, 8       # Update x3
    loop:
        beq x5, x0, exit    # If we have gone over all the numbers, then we can exit    
        ld x10, 0(x3)        # Load the first number of this pair
        ld x11, 8(x3)        # Load the second number of this pair
        
        addi sp, sp, -8
        sd x10, 8(sp)
        jal x1, fp64add      # Jump to the function
        sd x10, 0(x15)
        ld x10, 8(sp)
        addi, sp, sp, 8
        
        addi x15, x15, 8     # Update x15
        
        addi sp, sp, -8
        sd x10, 8(sp)
        jal x1, fp64mul
        sd x10, 0(x15)
        ld x10, 8(sp)
        addi, sp, sp, 8
        
        addi x15, x15, 8    # Update x15
        addi x5, x5, -1     # Update x5
        
        addi x3, x3, 16    # Move to the next pair of numbers
        beq x0, x0 loop
    
fp64add:
    and x18, x10, x12    # Mantissa of the first number
    and x19, x10, x13    # Shifted exponent of the first number
    and x20, x11, x12    # Mantissa of the second number
    and x21, x11, x13    # Shifted exponent of the second number
    
    # Handle denormal numbers
    
    ld x27, 24(sp)        # Load the value of number of bits for Mantissa
    ld x28 16(sp)         # Load the value of number of bits for exponent
    
    # First zero cases
    addi x26, x0, 1
    sll x26, x26, x27
    sll x26, x26, x28
    bne x19, x0, no_need_to_handle_zero
    bne x21, x0, no_need_to_handle_zero
    beq x0, x0, handle_zero_addition
    
    no_need_to_handle_zero:
    
    # Next inf or NaN case
        beq x19, x13, handle_inf_or_nan_addition
        beq x21, x13, handle_inf_or_nan_addition
    
    srl x19, x19, x27     # Bring the correct value of exponent
    srl x21, x21, x27
    
    addi x25, x0, 0      # This is to keep track of the rounding bit
    
    # Add the 1 hidden bit or check zero
    addi x26, x0, 1
    sll x26, x26, x27
    
    beq x19, x0, check_second_hidden_bit_add
    add x18, x18, x26
    check_second_hidden_bit_add:
    
    beq x21, x0, hidden_bit_add_correction
    add x20, x20, x26
    
    hidden_bit_add_correction:
        
    addi x30, x0, 1
    ld x31, 24(sp)
    sll x30, x30, x31
    ld x31, 16(sp)
    sll x30, x30, x31
    
    
    first_is_negative:
        and x31, x30, x10
        beq x31, x0, second_is_negative
        xori x18, x18, -1
        addi x18, x18, 1
            
    second_is_negative:
        and x31, x30, x11
        beq x31, x0, continue
        xori x20, x20, -1
        addi x20, x20, 1
    
    continue:    
    
    # Now lets compare the exponents
    blt x21, x19, if_second_exponent_smaller
    
    if_first_exponent_smaller:
        bge x19, x21, next
        sub x31, x21, x19
        addi x31, x31, -1
        sra x18, x18, x31
        andi x25, x18, 1
        srai x18, x18, 1
        add x19, x21, x0
        beq x0, x0, next
            
    if_second_exponent_smaller:   
        bge x21, x19, next
        sub x31, x19, x21
        addi x31, x31, -1
        sra x20, x20, x31
        andi x25, x20, 1
        srai x20, x20, 1
        add x21, x19, x0
        beq x0, x0, if_second_exponent_smaller
    next:
    
    add x22, x18, x20    # Add the mantissa
    add x23, x19, x0     # This is just to store the value of exponent for the result
    addi x24, x0, 0      # This is to store the signed bit
    
    beq x22, x0, addition_is_zero
    
    # Now need to check the sign
    bge x22, x0, result_is_positive
    
    addi x24, x24, 1
    xori x22, x22, -1
    addi x22, x22, 1
    
    result_is_positive:
        
    # Now need to normalize
    
    add x27, x26, x0
    addi x27, x27, -1
    add x27, x27, x26    # This is the upper limit, and x26 is the smaller limit
    
    addi x30, x0, 1
    ld x31, 16(sp)
    sll x30, x30, x31
    addi x30, x30, -1
    sub x31, x0, x14
    
    result_to_small:
        bge x22, x26, result_to_big
        beq x23, x31, return_from_addition_with_add        # Now this is denormal, we cannot reduce the exponent further, thus return for here itself
        slli x22, x22, 1
        addi x23, x23, -1
        beq x0, x0, result_to_small
        
        
    ld x30, 24(sp)
    ld x31, 16(sp)
    
    srl x30, x13, x30
        
    result_to_big:
        bge x23, x30, result_is_inf_add
        blt x22, x27 finish
        andi x25, x22, 1
        srli x22, x22, 1
        addi x23, x23, 1
        beq x0, x0 result_to_big
        
    finish:
        # Remove the hidden bit 1
        sub x22, x22, x26
        
        ld x30, 24(sp)
        ld x31, 16(sp)
        
        # Ensure everything shifted so it can be stored in the answer
        sll x24, x24, x30
        sll x24, x24, x31
        ld x26, 24(sp)
        
        srl x30, x13, x26
        bge x23, x30, result_is_inf_add
        
        sll x23, x23, x26
        
        # Add the carry bit
        add x22, x22, x25
        
        add x22, x22, x23
        add x22, x22, x24
        
        # Finally store the answer and return back
        beq x0, x0, return_from_addition
        
        result_is_inf_add:
            ld x30, 24(sp)
            ld x31, 16(sp)
            sll x24, x24, x30
            sll x24, x24, x31
            add x22, x13, x24
            beq x0, x0, return_from_addition
        
        handle_zero_addition:
            
            # If both numbers are denormal, directly add
            bne x19, x0, second_number_is_zero
            bne x21, x0, first_number_is_zero
            
            addi x31, x0, 1
            ld x30, 24(sp)
            sll x31, x31, x30
            ld x30, 16(sp)
            sll x31, x31, x30

            and x29, x31, x10
            and x30, x31, x11
            
            beq x29, x0, second_negative_case
            beq x30, x0, first_negative_case
            
            both_negative_case:
                add x22, x10, x11
                add x22, x22, x31
                beq x0, x0, return_from_addition
                
                first_negative_case:
                    add x28, x31, x10
                    blt x28, x11, second_greater_1
                    sub x22, x28, x11
                    add x22, x22, x31
                    beq x0, x0, return_from_addition
                 
                 second_greater_1:
                      sub x22, x11, x28
                      beq x0, x0, return_from_addition
                     
                second_negative_case:
                    beq x30, x0, both_positive_case
                    
                    add x28, x31, x11
                    blt x10, x28, second_greater_2
                    sub x22, x10, x28
                    beq x0, x0, return_from_addition
                 
                 second_greater_2:
                      sub x22, x28, x10
                      add x22, x22, x31
                      beq x0, x0, return_from_addition
                
                
            both_positive_case:
                add x22, x10, x11
                beq x0, x0, return_from_addition
              
            first_number_is_zero:
                beq x11, x0, second_number_is_zero
                beq x11, x26, second_number_is_zero
                
                addi x22, x11, 0
                beq x0, x0, return_from_addition
            second_number_is_zero:
                addi x22, x10, 0
                beq x0, x0, return_from_addition
                
        handle_inf_or_nan_addition:
            # First check NaN
            bne x19, x13, check_second_number_addition_nan
            beq x18, x0, check_second_number_addition_nan
            addi x22, x10, 0
            beq x0, x0, return_from_addition
            
            check_second_number_addition_nan:
                bne x21, x13, check_both_inf
                beq x20, x0, check_both_inf
                addi x22, x11, 0
                beq x0, x0, return_from_addition
            
            check_both_inf:
                bne x19, x13, second_is_inf
                bne x21, x13, first_is_inf
                
                bne x10, x11, is_sum_zero
                addi x22, x10, 0
                beq x0, x0, return_from_addition
                
                is_sum_zero:
                    addi x22, x0, 0
                    beq x0, x0, return_from_addition
                
            first_is_inf:
                addi x22, x10, 0
                beq x0, x0, return_from_addition
         
            second_is_inf:
                addi x22, x11, 0
                beq x0, x0, return_from_addition

            addition_is_zero:
                addi x22, x0, 0
                beq x0, x0, return_from_addition
            
        return_from_addition:    
            # Finally return to the callee
            add x10, x22, x0
            jalr x0, x1(0)
        
        return_from_addition_with_add:
            add x10, x22, x24
            jalr x0, x1(0)
fp64mul:
    and x18, x10, x12    # Mantissa of the first number
    and x19, x10, x13    # Shifted exponent of the first number
    and x20, x11, x12    # Mantissa of the second number
    and x21, x11, x13    # Shifted exponent of the second number
    
    ld x27, 24(sp)        # Load the value of number of bits for Mantissa
    
    # inf or NaN case
    beq x19, x13, handle_inf_or_nan_mult
    beq x21, x13, handle_inf_or_nan_mult
    
    # Zero case
    
    addi x26, x0, 1
    sll x26, x26, x27
    sll x26, x26, x28
    beq x10, x0, handle_zero_mult
    beq x11, x0, handle_zero_mult
    beq x10, x26, handle_zero_mult
    beq x11, x26, handle_zero_mult
    
    # Check both denormal, their product is 0
    bne x19, x0, skip
    bne x21, x0, skip
    beq x0, x0, handle_zero_mult
    
    skip:
    
    srl x19, x19, x27     # Bring the correct value of exponent
    srl x21, x21, x27
    
    # Add the 1 hidden bit or check zero
    addi x26, x0, 1
    sll x26, x26, x27
    
    beq x19, x0, check_second_hidden_bit_mul
    add x18, x18, x26
    check_second_hidden_bit_mul:
    
    beq x21, x0, hidden_bit_mul_correction
    add x20, x20, x26
    
    hidden_bit_mul_correction:
    
    addi sp, sp, -40
    sd x1, 40(sp)
    sd x10, 32(sp)
    sd x11, 24(sp)
    sd x12, 16(sp)
    sd x13, 8(sp)
    
    # Pass the function arguments to be sent to the dmult function
    addi x10, x18, 0
    addi x11, x20, 0
    jal x1, dmult
    
    # Pass the value in the x22 register
    addi x22, x10, 0
    addi x25, x11, 0
    
    # Now load back the values
    ld x1, 40(sp)
    ld x1, 40(sp)
    ld x10, 32(sp)
    ld x11, 24(sp)
    ld x12, 16(sp)
    ld x13, 8(sp)
    addi sp, sp, 40
    
    # The exponents are just added, but bias needs to be removed once
    add x23, x19, x21
    sub x23, x23, x14
    
    
    # Now need to normalize
    
    add x27, x26, x0
    addi x27, x27, -1
    add x27, x27, x26    # This is the upper limit, and x26 is the smaller limit
    
    addi x30, x0, 1
    ld x31, 16(sp)
    sll x30, x30, x31
    addi x30, x30, -1
    addi x31, x0, 1
    
    result_to_small_2:
        blt x23, x31, handle_zero_mult
        bge x22, x26, result_to_big_2
        slli x22, x22, 1
        addi x23, x23, -1
        beq x0, x0, result_to_small_2
        
    result_to_big_2:
        bge x23, x30, result_is_inf_mult
        blt x22, x27 finish_2
        andi x25, x22, 1
        srli x22, x22, 1
        addi x23, x23, 1
        beq x0, x0 result_to_big_2
        
    finish_2:
        # Remove the hidden bit 1
        sub x22, x22, x26
        
        # Ensure everything shifted so it can be stored in the answer
        ld x26, 24(sp)
        sll x23, x23, x26
        
        # Add the carry bit
        add x22, x22, x25
        
        add x22, x22, x23
        beq x0, x0, return_mult
        
        handle_inf_or_nan_mult:
            # First check NaN
            bne x19, x13, check_second_number_mult_nan
            beq x18, x0, check_second_number_mult_nan
            addi x22, x10, 0
            beq x0, x0, return_mult
            
            check_second_number_mult_nan:
                bne x21, x13, check_mult_inf
                beq x20, x0, check_mult_inf
                addi x22, x11, 0
                beq x0, x0, return_mult
            
            check_mult_inf:
                bne x19, x13, second_is_inf_mult
                bne x21, x13, first_is_inf_mult
                addi x22, x10, 0
                beq x0, x0, return_mult
                
            first_is_inf_mult:
                addi x22, x10, 0
                
                # Check second is zero or not
                ld x31, 24(sp)
                addi x30, x0, 1
                sll x30, x30, x31
                ld x31, 16(sp)
                sll x30, x30, x31
                
                beq x11, x0, is_nan_1
                beq x11, x30, is_nan_1
                
                beq x0, x0, return_mult

                is_nan_1:
                    addi x22, x10, 1
                    beq x0, x0, return_mult
            second_is_inf_mult:
                addi x22, x11, 0
                
                # Check first is zero or not
                ld x31, 24(sp)
                addi x30, x0, 1
                sll x30, x30, x31
                ld x31, 16(sp)
                sll x30, x30, x31
                
                beq x10, x0, is_nan_2
                beq x10, x30, is_nan_2
                
                beq x0, x0, return_mult
                
                is_nan_2:
                    addi x22, x11, 1
                    beq x0, x0, return_mult
            
            handle_zero_mult:
                addi x22, x0, 0
                beq x0, x0, return_mult
                
        result_is_inf_mult:
        addi x22, x13, 0
        
        return_mult:
            # The signed bit is stored in x24
            addi x24, x0, 0
    
            # Now if both values are positive or negative signed bit is 1, else 0
            check_first_positive:
                addi x30, x0, 1
                ld x31, 24(sp)
                sll x30, x30, x31
                ld x31, 16(sp)
                sll x30, x30, x31
                
                and x31, x30, x10
                beq x31, x0, check_second_positive
                addi x24, x24, 1
        
            check_second_positive:
                and x31, x30, x11
                beq x31, x0, done_checking_sign
                addi x24, x24, 1
            done_checking_sign:
                ld x30, 24(sp)
                ld x31, 16(sp)
                sll x24, x24, x30
                sll x24, x24, x31
            
            # Ensure x22 does not have a signed bit
            slli x22, x22, 1
            srli x22, x22, 1
            
            # Add the sign to the result
            add x22, x22, x24
            
            # Finally return the number in the register x10
            add x10, x22, x0
        
            # Finally return to the callee
            jalr x0, x1(0)
    
    
dmult:
    addi x12, x0, 0    # This is to store the rounding bit
    addi x13, x0, 0    # Also let us use the x13 register to store the answer for now
    ld x30, 64(sp)
    addi x30, x30, 1
    # I am using x31 as a dummy register
    while_muliplier_is_zero:
        beq x30, x0, mult_done
        addi x30, x30, -1
        andi x12, x13, 1
        srli x13, x13, 1
        next_step_for_mul:
        andi x31, x11, 1
        srli x11, x11, 1
        beq x31, x0, while_muliplier_is_zero
        add x13, x13, x10
        beq x0, x0, while_muliplier_is_zero
        
    mult_done:
        # Now we need to return the rounding bit and the answer
        addi x10, x13, 0
        addi x11, x12, 0
        jalr x0, x1(0)

report_error:
      la a0, error          # load the address of hello into $a0 (1st argument)
      addi a7, zero, 4      # code to print the string at the address a0
      ecall                 # make the system call
        
exit:
    beq x0, x0, exit     # Infinite loop after you finish
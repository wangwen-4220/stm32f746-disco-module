
    .text
    .align 4
    .syntax unified

    .global _gcc_setup
    .thumb_func
_gcc_setup:

    STMDB   sp!, {r3, r4, r5, r6, r7, lr}             // Store other preserved registers

    ldr     r3, =__FLASH_segment_start__
    ldr     r4, =__RAM_segment_start__
    mov     r5,r0

    /* Copy GOT table. */
  
    ldr     r0, =__got_load_start__
    sub     r0,r0,r3
    add     r0,r0,r5
    ldr     r1, =__new_got_start__
    sub     r1,r1, r4
    add     r1,r1,r9
    ldr     r2, =__new_got_end__
    sub     r2,r2,r4
    add     r2,r2,r9

new_got_setup:
    cmp     r1, r2          // See if there are more GOT entries
    beq     got_setup_done  // No, done with GOT setup
    ldr     r6, [r0]        // Pickup current GOT entry
    cmp     r6, #0          // Is it 0?
    beq     address_built   // Yes, just skip the adjustment
    cmp     r6, r4          // Is it in the code or data area?
    blt     flash_area      // If less than, it is a code address
    sub     r6, r6, r4      // Compute offset of data area
    add     r6, r6, r9      // Build address based on the loaded data address
    b       address_built   // Finished building address
flash_area:
    sub     r6, r6, r3      // Compute offset of code area
    add     r6, r6, r5      // Build address based on the loaded code address
address_built:
    str     r6, [r1]        // Store in new GOT table
    add     r0, r0, #4      // Move to next entry
    add     r1, r1, #4      // 
    b       new_got_setup   // Continue at the top of the loop
got_setup_done:


    /* Copy initialised sections into RAM if required. */
  
    ldr     r0, =__data_load_start__
    sub     r0,r0,r3
    add     r0,r0,r5
    ldr     r1, =__data_start__
    sub     r1,r1, r4
    add     r1,r1,r9
    ldr     r2, =__data_end__
    sub     r2,r2,r4
    add     r2,r2,r9
    bl      crt0_memory_copy
  
    /* Zero bss. */
    
    ldr     r0, =__bss_start__
    sub     r0,r0,r4
    add     r0,r0,r9
    ldr     r1, =__bss_end__
    sub     r1,r1,r4
    add     r1,r1,r9
    mov     r2, #0
    bl      crt0_memory_set


    /* Setup heap - not recommended for Threadx but here for compatibility reasons */

    ldr     r0, =__heap_start__
    sub     r0,r0,r4
    add     r0,r0,r9
    ldr     r1, =__heap_end__
    sub     r1,r1,r4
    add     r1,r1,r9
    sub     r1,r1,r0
    mov     r2, #0
    str     r2, [r0]
    add     r0, r0, #4
    str     r1, [r0]
    
    LDMIA   sp!, {r3, r4, r5, r6, r7, lr}       // Store other preserved registers
    bx      lr                                  // Return to caller
  
    .align 4

  /* Startup helper functions. */

    .thumb_func
crt0_memory_copy:

    cmp     r0, r1
    beq     memory_copy_done
    cmp     r2, r1
    beq     memory_copy_done
    sub     r2, r2, r1
memory_copy_loop:
    ldrb    r3, [r0]
    add     r0, r0, #1
    strb    r3, [r1]
    add     r1, r1, #1
    sub     r2, r2, #1
    cmp     r2, #0
    bne     memory_copy_loop
memory_copy_done:
    bx      lr

    .thumb_func
crt0_memory_set:
    cmp     r0, r1
    beq     memory_set_done
    strb    r2, [r0]
    add     r0, r0, #1
    b       crt0_memory_set
memory_set_done:
    //bx      lr

/*relocate rel.dyn */
/*******************************************/

rel_dyn_relocate:
	ldr     r3, =__FLASH_segment_start__
	ldr	r2, =__reldyn_load_start__	/* r2 <-  __rel_dyn_start */
	sub     r2, r2, r3
    	add     r2, r2, r5

	ldr	r6, =__reldyn_end__	       /* r6 <-  __rel_dyn_end */
	sub     r6, r6, r3
    	add     r6, r6, r5

fixloop:
	ldmia	r2!, {r0-r1}		/* (r0,r1) <- (r2) */
	and	r1, r1, #0xff
	cmp	r1, #23  		/*R_ARM_RELATIVE = 23*/
	bne	fixnext

/* step1.needs to judge if r0 is ram address */

	cmp     r0, r4
	blt     fixnext         	// If less than, it is a flash address,we do nothing
    	sub     r0, r0, r4      	// Compute offset of data area
    	add     r0, r0, r9      	// Build address based on the loaded data address
	ldr	r1, [r0]        	// get r0 content

/* step2. needs to caculate  the content of r0 address offset */
/* relative fix: increase location by offset */

	cmp     r1, r4
	blt     flash_offset	// If less than, it is a flash address
	sub     r1, r1, r4      // Compute offset of data area
    	add     r1, r1, r9      // Build address based on the loaded data address
	str	r1, [r0]  	//update r0 content
	b 	fixnext

flash_offset:
	sub     r1, r1, r3      // Compute offset of code area
	add     r1, r1, r5      // Build address based on the loaded code address
	str	r1, [r0]

fixnext:
	cmp	r2, r6
	blo	fixloop 	//jump when r2 < r6

relocate_done:
    	bx      lr

/*******************************************/

    /* Setup attibutes of heap section so it doesn't take up room in the elf file */
    .section .heap, "wa", %nobits
  

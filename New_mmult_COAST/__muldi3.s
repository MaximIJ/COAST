.text
	.align	2
	.globl	__muldi3
	.type	__muldi3, @function
__muldi3:
	# a0, a1 are the two 64-bit values to multiply
	# We'll return the result in a0
	
	# Save registers we'll use
	addi	sp, sp, -48
	sd	ra, 40(sp)
	sd	s0, 32(sp)
	sd	s1, 24(sp)
	sd	s2, 16(sp)
	sd	s3, 8(sp)
	sd	s4, 0(sp)
	
	# Check for simple cases first to optimize matrix multiplication patterns
	beqz	a0, .L_return_zero    # If a0 == 0, return 0
	beqz	a1, .L_return_zero    # If a1 == 0, return 0
	
	li	t0, 1
	beq	a0, t0, .L_return_a1  # If a0 == 1, return a1
	beq	a1, t0, .L_return_a0  # If a1 == 1, return a0
	
	# For matrix multiplications, we often multiply small integers
	# Check if both inputs fit in 32 bits (common in many matrices)
	li	t0, 0xffffffff
	and	t1, a0, t0          # Lower 32 bits of a0
	srl	t2, a0, 32          # Upper 32 bits of a0
	beqz	t2, .L_a0_is_32bit
	
	# General case - standard algorithm for 64-bit multiplication
	mv	s0, a0              # Save a0 in s0
	li	a0, 0               # Initialize result to 0
	
.L_loop:
	andi	t0, a1, 1           # Check lowest bit of a1
	beqz	t0, .L_skip_add
	add	a0, a0, s0          # Add s0 to result if bit is set
	
.L_skip_add:
	srli	a1, a1, 1           # Shift a1 right by 1 bit
	slli	s0, s0, 1           # Shift s0 left by 1 bit
	bnez	a1, .L_loop         # Continue if a1 != 0
	j	.L_done
	
.L_a0_is_32bit:
	and	t3, a1, t0          # Lower 32 bits of a1
	srl	t4, a1, 32          # Upper 32 bits of a1
	beqz	t4, .L_both_32bit
	
	# a0 is 32-bit but a1 is 64-bit
	mul	t0, t1, t3          # Low part * Low part
	mul	t1, t1, t4          # Low part * High part
	slli	t1, t1, 32
	add	a0, t0, t1
	j	.L_done
	
.L_both_32bit:
	# Both inputs fit in 32 bits - use direct multiplication
	mul	a0, t1, t3
	j	.L_done
	
.L_return_zero:
	li	a0, 0
	j	.L_done
	
.L_return_a1:
	mv	a0, a1
	j	.L_done
	
.L_return_a0:
	# a0 is already in a0
	
.L_done:
	# Restore saved registers
	ld	s4, 0(sp)
	ld	s3, 8(sp)
	ld	s2, 16(sp)
	ld	s1, 24(sp)
	ld	s0, 32(sp)
	ld	ra, 40(sp)
	addi	sp, sp, 48
	ret
	
	.size	__muldi3, .-__muldi3

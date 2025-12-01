.text
	.file	"matrixMultiply.c"
        # Reference the external variable defined in C
        .extern tmr_mitigations
	.globl	matrix_multiply         # -- Begin function matrix_multiply
	.p2align	2
	.type	matrix_multiply,@function
matrix_multiply:                        # @matrix_multiply
# %bb.0:
	addi	sp, sp, -256
	sd	ra, 248(sp)
	sd	s0, 240(sp)
	sd	s1, 232(sp)
	sd	s2, 224(sp)
	sd	s3, 216(sp)
	sd	s4, 208(sp)
	sd	s5, 200(sp)
	sd	s6, 192(sp)
	sd	s7, 184(sp)
	sd	s8, 176(sp)
	sd	s9, 168(sp)
	sd	s10, 160(sp)
	sd	s11, 152(sp)
	addi	s0, sp, 256
	sd	a0, -112(s0)
	sd	a1, -120(s0)
	sd	a2, -128(s0)
	sd	a0, -160(s0)
	sd	a1, -168(s0)
	sd	a2, -176(s0)
	sd	a0, -208(s0)
	sd	a1, -216(s0)
	sd	a2, -224(s0)
	sw	zero, -132(s0)
	sw	zero, -180(s0)
	sw	zero, -228(s0)
	sw	zero, -136(s0)
	sw	zero, -184(s0)
	sw	zero, -232(s0)
	sw	zero, -140(s0)
	sw	zero, -188(s0)
	sw	zero, -236(s0)
	sd	zero, -152(s0)
	sd	zero, -200(s0)
	sd	zero, -248(s0)
	sw	zero, -132(s0)
	sw	zero, -180(s0)
	sw	zero, -228(s0)
	j	.LBB0_1
.LBB0_1:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_5 Depth 2
                                        #       Child Loop BB0_9 Depth 3
	lw	a0, -132(s0)
	slti	a0, a0, 64            # Changed from 9 to 64 for i < MATRIX_SIZE
	lw	a1, -180(s0)
	slti	a2, a1, 64            # Changed from 9 to 64
	lw	a1, -228(s0)
	slti	a1, a1, 64            # Changed from 9 to 64
	
	# TMR voting logic for loop condition check
	# This is an example - you should modify based on your actual TMR approach
	beq	a0, a2, .L_no_error_1   # If first two agree, skip error handling
	beq	a0, a1, .L_mitigation_1 # First and third agree, error in second
	beq	a1, a2, .L_mitigation_1 # Second and third agree, error in first
	j	.LBB0_3                  # No majority, cannot mitigate
	
.L_mitigation_1:
	# Increment the mitigation counter
	la	t0, tmr_mitigations
	lw	t1, 0(t0)
	addi	t1, t1, 1
	sw	t1, 0(t0)
	
.L_no_error_1:
	xor	a2, a0, a2
	bnez	a2, .LBB0_3
# %bb.2:                                #   in Loop: Header=BB0_1 Depth=1
	mv	a1, a0
.LBB0_3:                                #   in Loop: Header=BB0_1 Depth=1
	addi	a0, zero, 1
	bne	a1, a0, .LBB0_18
	j	.LBB0_4
.LBB0_4:                                #   in Loop: Header=BB0_1 Depth=1
	sw	zero, -136(s0)
	sw	zero, -184(s0)
	sw	zero, -232(s0)
	j	.LBB0_5
.LBB0_5:                                #   Parent Loop BB0_1 Depth=1
                                        # =>  This Loop Header: Depth=2
                                        #       Child Loop BB0_9 Depth 3
	lw	a0, -136(s0)
	slti	a0, a0, 64            # Changed from 9 to 64 for j < MATRIX_SIZE
	lw	a1, -184(s0)
	slti	a2, a1, 64            # Changed from 9 to 64
	lw	a1, -232(s0)
	slti	a1, a1, 64            # Changed from 9 to 64
	
	# TMR voting logic for inner loop condition
	beq	a0, a2, .L_no_error_2   # If first two agree, skip error handling
	beq	a0, a1, .L_mitigation_2 # First and third agree, error in second
	beq	a1, a2, .L_mitigation_2 # Second and third agree, error in first
	j	.LBB0_7                  # No majority, cannot mitigate
	
.L_mitigation_2:
	# Increment the mitigation counter
	la	t0, tmr_mitigations
	lw	t1, 0(t0)
	addi	t1, t1, 1
	sw	t1, 0(t0)
	
.L_no_error_2:
	xor	a2, a0, a2
	bnez	a2, .LBB0_7
# %bb.6:                                #   in Loop: Header=BB0_5 Depth=2
	mv	a1, a0
.LBB0_7:                                #   in Loop: Header=BB0_5 Depth=2
	addi	a0, zero, 1
	bne	a1, a0, .LBB0_16
	j	.LBB0_8
.LBB0_8:                                #   in Loop: Header=BB0_5 Depth=2
	sw	zero, -140(s0)
	sw	zero, -188(s0)
	sw	zero, -236(s0)
	j	.LBB0_9
.LBB0_9:                                #   Parent Loop BB0_1 Depth=1
                                        #     Parent Loop BB0_5 Depth=2
                                        # =>    This Inner Loop Header: Depth=3
	lw	a0, -140(s0)
	slti	a0, a0, 64            # Changed from 9 to 64 for k < MATRIX_SIZE
	lw	a1, -188(s0)
	slti	a2, a1, 64            # Changed from 9 to 64
	lw	a1, -236(s0)
	slti	a1, a1, 64            # Changed from 9 to 64
	
	# TMR voting logic for innermost loop
	beq	a0, a2, .L_no_error_3   # If first two agree, skip error handling
	beq	a0, a1, .L_mitigation_3 # First and third agree, error in second
	beq	a1, a2, .L_mitigation_3 # Second and third agree, error in first
	j	.LBB0_11                 # No majority, cannot mitigate
	
.L_mitigation_3:
	# Increment the mitigation counter
	la	t0, tmr_mitigations
	lw	t1, 0(t0)
	addi	t1, t1, 1
	sw	t1, 0(t0)
	
.L_no_error_3:
	xor	a2, a0, a2
	bnez	a2, .LBB0_11
# %bb.10:                               #   in Loop: Header=BB0_9 Depth=3
	mv	a1, a0
.LBB0_11:                               #   in Loop: Header=BB0_9 Depth=3
	addi	a0, zero, 1
	bne	a1, a0, .LBB0_14
	j	.LBB0_12
.LBB0_12:                               #   in Loop: Header=BB0_9 Depth=3
	ld	s8, -152(s0)
	ld	s3, -112(s0)
	lw	a0, -132(s0)
	ld	s7, -200(s0)
	ld	s4, -160(s0)
	lw	s1, -180(s0)
	ld	s6, -248(s0)
	ld	s5, -208(s0)
	lw	s2, -228(s0)
	addi	a1, zero, 512          # Changed from 36 to 512 (64*8 bytes per row for int64_t)
	call	__muldi3
	add	s3, s3, a0
	mv	a0, s1
	addi	a1, zero, 512          # Changed from 36 to 512
	call	__muldi3
	add	s4, s4, a0
	mv	a0, s2
	addi	a1, zero, 512          # Changed from 36 to 512
	call	__muldi3
	add	a1, s5, a0
	lw	a0, -140(s0)
	lw	s1, -188(s0)
	lw	s2, -236(s0)
	slli	a2, a0, 3              # Changed from 2 to 3 (shifting by 3 for 8-byte int64_t)
	add	a2, s3, a2
	slli	a3, s1, 3              # Changed from 2 to 3
	add	a3, s4, a3
	slli	a4, s2, 3              # Changed from 2 to 3
	add	a1, a1, a4
	ld	s3, 0(a2)              # Changed from lw to ld for 64-bit integers
	ld	s9, -120(s0)
	ld	s4, 0(a3)              # Changed from lw to ld
	ld	s10, -168(s0)
	ld	s5, 0(a1)              # Changed from lw to ld
	ld	s11, -216(s0)
	addi	a1, zero, 512          # Changed from 36 to 512
	call	__muldi3
	add	s9, s9, a0
	mv	a0, s1
	addi	a1, zero, 512          # Changed from 36 to 512
	call	__muldi3
	add	s1, s10, a0
	mv	a0, s2
	addi	a1, zero, 512          # Changed from 36 to 512
	call	__muldi3
	add	a0, s11, a0
	lw	a1, -136(s0)
	lw	a2, -184(s0)
	lw	a3, -232(s0)
	slli	a1, a1, 3              # Changed from 2 to 3
	add	a1, s9, a1
	slli	a2, a2, 3              # Changed from 2 to 3
	add	s1, s1, a2
	slli	a2, a3, 3              # Changed from 2 to 3
	add	s2, a0, a2
	ld	a1, 0(a1)              # Changed from lw to ld
	mv	a0, s3
	call	__muldi3
	add	s3, s8, a0             # No need to use sext.w for 64-bit values
	ld	a1, 0(s1)              # Changed from lw to ld
	mv	a0, s4
	call	__muldi3
	add	s1, s7, a0             # No need to use sext.w
	ld	a1, 0(s2)              # Changed from lw to ld
	mv	a0, s5
	call	__muldi3
	add	a0, s6, a0             # No need to use sext.w
	
	# TMR for computation results - simplified example
	beq	s3, s1, .L_same_result_1  # First and second copy match
	beq	s3, a0, .L_mitigation_4   # First and third match, error in second
	beq	s1, a0, .L_mitigation_4   # Second and third match, error in first
	j	.L_store_results           # No majority, cannot mitigate
	
.L_mitigation_4:
	# Increment the mitigation counter
	la	t0, tmr_mitigations
	lw	t1, 0(t0)
	addi	t1, t1, 1
	sw	t1, 0(t0)
	j	.L_store_results
	
.L_same_result_1:
	# No error detected (first and second match), just continue

.L_store_results:
	sd	s3, -152(s0)
	sd	s1, -200(s0)
	sd	a0, -248(s0)
	j	.LBB0_13
.LBB0_13:                               #   in Loop: Header=BB0_9 Depth=3
	lw	a0, -140(s0)
	addi	a0, a0, 1
	lw	a1, -188(s0)
	addi	a1, a1, 1
	lw	a2, -236(s0)
	addi	a2, a2, 1
	sw	a0, -140(s0)
	sw	a1, -188(s0)
	sw	a2, -236(s0)
	j	.LBB0_9
.LBB0_14:                               #   in Loop: Header=BB0_5 Depth=2
	ld	s3, -152(s0)
	ld	s4, -128(s0)
	lw	a0, -132(s0)
	ld	s5, -200(s0)
	ld	s6, -176(s0)
	lw	s1, -180(s0)
	ld	s7, -248(s0)
	ld	s8, -224(s0)
	lw	s2, -228(s0)
	addi	a1, zero, 512          # Changed from 36 to 512
	call	__muldi3
	add	s4, s4, a0
	mv	a0, s1
	addi	a1, zero, 512          # Changed from 36 to 512
	call	__muldi3
	add	s1, s6, a0
	mv	a0, s2
	addi	a1, zero, 512          # Changed from 36 to 512
	call	__muldi3
	add	a0, s8, a0
	lw	a1, -136(s0)
	lw	a2, -184(s0)
	lw	a3, -232(s0)
	slli	a1, a1, 3              # Changed from 2 to 3
	add	a1, s4, a1
	slli	a2, a2, 3              # Changed from 2 to 3
	add	a2, s1, a2
	slli	a3, a3, 3              # Changed from 2 to 3
	add	a0, a0, a3
	
	# TMR for the final results - simplified example
	beq	s3, s5, .L_same_result_2   # First and second copy match
	beq	s3, s7, .L_mitigation_5    # First and third match, error in second
	beq	s5, s7, .L_mitigation_5    # Second and third match, error in first
	j	.L_store_final              # No majority, cannot mitigate
	
.L_mitigation_5:
	# Increment the mitigation counter
	la	t0, tmr_mitigations
	lw	t1, 0(t0)
	addi	t1, t1, 1
	sw	t1, 0(t0)
	j	.L_store_final
	
.L_same_result_2:
	# No error detected (first and second match), just continue

.L_store_final:
	sd	s3, 0(a1)              # Changed from sw to sd
	sd	s5, 0(a2)              # Changed from sw to sd
	sd	s7, 0(a0)              # Changed from sw to sd
	sd	zero, -152(s0)
	sd	zero, -200(s0)
	sd	zero, -248(s0)
	j	.LBB0_15
.LBB0_15:                               #   in Loop: Header=BB0_5 Depth=2
	lw	a0, -136(s0)
	addi	a0, a0, 1
	lw	a1, -184(s0)
	addi	a1, a1, 1
	lw	a2, -232(s0)
	addi	a2, a2, 1
	sw	a0, -136(s0)
	sw	a1, -184(s0)
	sw	a2, -232(s0)
	j	.LBB0_5
.LBB0_16:                               #   in Loop: Header=BB0_1 Depth=1
	j	.LBB0_17
.LBB0_17:                               #   in Loop: Header=BB0_1 Depth=1
	lw	a0, -132(s0)
	addi	a0, a0, 1
	lw	a1, -180(s0)
	addi	a1, a1, 1
	lw	a2, -228(s0)
	addi	a2, a2, 1
	sw	a0, -132(s0)
	sw	a1, -180(s0)
	sw	a2, -228(s0)
	j	.LBB0_1
.LBB0_18:
	ld	s11, 152(sp)
	ld	s10, 160(sp)
	ld	s9, 168(sp)
	ld	s8, 176(sp)
	ld	s7, 184(sp)
	ld	s6, 192(sp)
	ld	s5, 200(sp)
	ld	s4, 208(sp)
	ld	s3, 216(sp)
	ld	s2, 224(sp)
	ld	s1, 232(sp)
	ld	s0, 240(sp)
	ld	ra, 248(sp)
	addi	sp, sp, 256
	ret
.Lfunc_end0:
	.size	matrix_multiply, .Lfunc_end0-matrix_multiply
                                        # -- End function
	.ident	"clang version 7.0.0-3~ubuntu0.18.04.1 (tags/RELEASE_700/final)"
	.section	".note.GNU-stack","",@progbits

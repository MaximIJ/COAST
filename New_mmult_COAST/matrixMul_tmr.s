	.text
	.attribute	4, 16
	.attribute	5, "rv64i2p0"
	.file	"mm_tmr.c"
	.globl	testing                         # -- Begin function testing
	.p2align	1
	.type	testing,@function
testing:                                # @testing
# %bb.0:
	addi	sp, sp, -112
	sd	ra, 104(sp)                     # 8-byte Folded Spill
	sd	s0, 96(sp)                      # 8-byte Folded Spill
	sd	s1, 88(sp)                      # 8-byte Folded Spill
	sd	s2, 80(sp)                      # 8-byte Folded Spill
	sd	s3, 72(sp)                      # 8-byte Folded Spill
	sd	s4, 64(sp)                      # 8-byte Folded Spill
	sd	s5, 56(sp)                      # 8-byte Folded Spill
	sd	s6, 48(sp)                      # 8-byte Folded Spill
	addi	s0, sp, 112
	sw	zero, -68(s0)
	sw	zero, -84(s0)
	sw	zero, -100(s0)
	j	.LBB0_1
.LBB0_1:                                # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_7 Depth 2
                                        #       Child Loop BB0_13 Depth 3
	lw	a0, -68(s0)
	slti	a0, a0, 64
	lw	a1, -84(s0)
	slti	a2, a1, 64
	lw	a1, -100(s0)
	slti	a1, a1, 64
	xor	a2, a2, a0
	xori	a2, a2, 1
	xor	a3, a0, a1
	xori	a3, a3, 1
	and	a3, a3, a2
	bnez	a3, .LBB0_3
	j	.LBB0_2
.LBB0_2:                                # %errorHandler.testing
                                        #   in Loop: Header=BB0_1 Depth=1
.Lpcrel_hi0:
	auipc	a3, %pcrel_hi(TMR_ERROR_CNT)
	addi	a3, a3, %pcrel_lo(.Lpcrel_hi0)
	lw	a4, 0(a3)
	addiw	a4, a4, 1
	sw	a4, 0(a3)
	j	.LBB0_3
.LBB0_3:                                # %testing.cont
                                        #   in Loop: Header=BB0_1 Depth=1
	andi	a2, a2, 1
	bnez	a2, .LBB0_5
# %bb.4:                                # %testing.cont
                                        #   in Loop: Header=BB0_1 Depth=1
	mv	a0, a1
.LBB0_5:                                # %testing.cont
                                        #   in Loop: Header=BB0_1 Depth=1
	andi	a0, a0, 1
	beqz	a0, .LBB0_61
	j	.LBB0_6
.LBB0_6:                                #   in Loop: Header=BB0_1 Depth=1
	sw	zero, -72(s0)
	sw	zero, -88(s0)
	sw	zero, -104(s0)
	j	.LBB0_7
.LBB0_7:                                #   Parent Loop BB0_1 Depth=1
                                        # =>  This Loop Header: Depth=2
                                        #       Child Loop BB0_13 Depth 3
	lw	a0, -72(s0)
	slti	a0, a0, 64
	lw	a1, -88(s0)
	slti	a2, a1, 64
	lw	a1, -104(s0)
	slti	a1, a1, 64
	xor	a2, a2, a0
	xori	a2, a2, 1
	xor	a3, a0, a1
	xori	a3, a3, 1
	and	a3, a3, a2
	bnez	a3, .LBB0_9
	j	.LBB0_8
.LBB0_8:                                # %errorHandler.testing101
                                        #   in Loop: Header=BB0_7 Depth=2
.Lpcrel_hi1:
	auipc	a3, %pcrel_hi(TMR_ERROR_CNT)
	addi	a3, a3, %pcrel_lo(.Lpcrel_hi1)
	lw	a4, 0(a3)
	addiw	a4, a4, 1
	sw	a4, 0(a3)
	j	.LBB0_9
.LBB0_9:                                # %testing.cont104
                                        #   in Loop: Header=BB0_7 Depth=2
	andi	a2, a2, 1
	bnez	a2, .LBB0_11
# %bb.10:                               # %testing.cont104
                                        #   in Loop: Header=BB0_7 Depth=2
	mv	a0, a1
.LBB0_11:                               # %testing.cont104
                                        #   in Loop: Header=BB0_7 Depth=2
	andi	a0, a0, 1
	beqz	a0, .LBB0_55
	j	.LBB0_12
.LBB0_12:                               #   in Loop: Header=BB0_7 Depth=2
	sw	zero, -80(s0)
	sw	zero, -96(s0)
	sw	zero, -112(s0)
	sw	zero, -76(s0)
	sw	zero, -92(s0)
	sw	zero, -108(s0)
	j	.LBB0_13
.LBB0_13:                               #   Parent Loop BB0_1 Depth=1
                                        #     Parent Loop BB0_7 Depth=2
                                        # =>    This Inner Loop Header: Depth=3
	lw	a0, -76(s0)
	slti	a0, a0, 64
	lw	a1, -92(s0)
	slti	a2, a1, 64
	lw	a1, -108(s0)
	slti	a1, a1, 64
	xor	a2, a2, a0
	xori	a2, a2, 1
	xor	a3, a0, a1
	xori	a3, a3, 1
	and	a3, a3, a2
	bnez	a3, .LBB0_15
	j	.LBB0_14
.LBB0_14:                               # %errorHandler.testing109
                                        #   in Loop: Header=BB0_13 Depth=3
.Lpcrel_hi2:
	auipc	a3, %pcrel_hi(TMR_ERROR_CNT)
	addi	a3, a3, %pcrel_lo(.Lpcrel_hi2)
	lw	a4, 0(a3)
	addiw	a4, a4, 1
	sw	a4, 0(a3)
	j	.LBB0_15
.LBB0_15:                               # %testing.cont112
                                        #   in Loop: Header=BB0_13 Depth=3
	andi	a2, a2, 1
	bnez	a2, .LBB0_17
# %bb.16:                               # %testing.cont112
                                        #   in Loop: Header=BB0_13 Depth=3
	mv	a0, a1
.LBB0_17:                               # %testing.cont112
                                        #   in Loop: Header=BB0_13 Depth=3
	andi	a0, a0, 1
	beqz	a0, .LBB0_45
	j	.LBB0_18
.LBB0_18:                               #   in Loop: Header=BB0_13 Depth=3
	lw	a0, -68(s0)
	addi	a0, a0, -6
	seqz	a0, a0
	lw	a1, -84(s0)
	addi	a1, a1, -6
	seqz	a2, a1
	lw	a1, -100(s0)
	addi	a1, a1, -6
	seqz	a1, a1
	xor	a2, a2, a0
	xori	a2, a2, 1
	xor	a3, a0, a1
	xori	a3, a3, 1
	and	a3, a3, a2
	bnez	a3, .LBB0_20
	j	.LBB0_19
.LBB0_19:                               # %errorHandler.testing117
                                        #   in Loop: Header=BB0_13 Depth=3
.Lpcrel_hi3:
	auipc	a3, %pcrel_hi(TMR_ERROR_CNT)
	addi	a3, a3, %pcrel_lo(.Lpcrel_hi3)
	lw	a4, 0(a3)
	addiw	a4, a4, 1
	sw	a4, 0(a3)
	j	.LBB0_20
.LBB0_20:                               # %testing.cont120
                                        #   in Loop: Header=BB0_13 Depth=3
	andi	a2, a2, 1
	bnez	a2, .LBB0_22
# %bb.21:                               # %testing.cont120
                                        #   in Loop: Header=BB0_13 Depth=3
	mv	a0, a1
.LBB0_22:                               # %testing.cont120
                                        #   in Loop: Header=BB0_13 Depth=3
	andi	a0, a0, 1
	beqz	a0, .LBB0_34
	j	.LBB0_23
.LBB0_23:                               #   in Loop: Header=BB0_13 Depth=3
	lw	a0, -72(s0)
	addi	a0, a0, -6
	seqz	a0, a0
	lw	a1, -88(s0)
	addi	a1, a1, -6
	seqz	a2, a1
	lw	a1, -104(s0)
	addi	a1, a1, -6
	seqz	a1, a1
	xor	a2, a2, a0
	xori	a2, a2, 1
	xor	a3, a0, a1
	xori	a3, a3, 1
	and	a3, a3, a2
	bnez	a3, .LBB0_25
	j	.LBB0_24
.LBB0_24:                               # %errorHandler.testing125
                                        #   in Loop: Header=BB0_13 Depth=3
.Lpcrel_hi4:
	auipc	a3, %pcrel_hi(TMR_ERROR_CNT)
	addi	a3, a3, %pcrel_lo(.Lpcrel_hi4)
	lw	a4, 0(a3)
	addiw	a4, a4, 1
	sw	a4, 0(a3)
	j	.LBB0_25
.LBB0_25:                               # %testing.cont128
                                        #   in Loop: Header=BB0_13 Depth=3
	andi	a2, a2, 1
	bnez	a2, .LBB0_27
# %bb.26:                               # %testing.cont128
                                        #   in Loop: Header=BB0_13 Depth=3
	mv	a0, a1
.LBB0_27:                               # %testing.cont128
                                        #   in Loop: Header=BB0_13 Depth=3
	andi	a0, a0, 1
	beqz	a0, .LBB0_34
	j	.LBB0_28
.LBB0_28:                               #   in Loop: Header=BB0_13 Depth=3
	lw	a0, -76(s0)
	addi	a0, a0, -6
	seqz	a0, a0
	lw	a1, -92(s0)
	addi	a1, a1, -6
	seqz	a2, a1
	lw	a1, -108(s0)
	addi	a1, a1, -6
	seqz	a1, a1
	xor	a2, a2, a0
	xori	a2, a2, 1
	xor	a3, a0, a1
	xori	a3, a3, 1
	and	a3, a3, a2
	bnez	a3, .LBB0_30
	j	.LBB0_29
.LBB0_29:                               # %errorHandler.testing133
                                        #   in Loop: Header=BB0_13 Depth=3
.Lpcrel_hi5:
	auipc	a3, %pcrel_hi(TMR_ERROR_CNT)
	addi	a3, a3, %pcrel_lo(.Lpcrel_hi5)
	lw	a4, 0(a3)
	addiw	a4, a4, 1
	sw	a4, 0(a3)
	j	.LBB0_30
.LBB0_30:                               # %testing.cont136
                                        #   in Loop: Header=BB0_13 Depth=3
	andi	a2, a2, 1
	bnez	a2, .LBB0_32
# %bb.31:                               # %testing.cont136
                                        #   in Loop: Header=BB0_13 Depth=3
	mv	a0, a1
.LBB0_32:                               # %testing.cont136
                                        #   in Loop: Header=BB0_13 Depth=3
	andi	a0, a0, 1
	beqz	a0, .LBB0_34
	j	.LBB0_33
.LBB0_33:                               #   in Loop: Header=BB0_13 Depth=3
	li	a0, -1
	sw	a0, -80(s0)
	li	a0, 0
	sw	a0, -96(s0)
	li	a0, 1
	sw	a0, -112(s0)
	j	.LBB0_39
.LBB0_34:                               #   in Loop: Header=BB0_13 Depth=3
	lw	a0, -68(s0)
	lw	a1, -84(s0)
	lw	a2, -100(s0)
	slli	a0, a0, 9
.Lpcrel_hi6:
	auipc	a3, %pcrel_hi(matrix_a)
	addi	a3, a3, %pcrel_lo(.Lpcrel_hi6)
	add	a0, a0, a3
	slli	a1, a1, 9
.Lpcrel_hi7:
	auipc	a3, %pcrel_hi(matrix_a_DWC)
	addi	a3, a3, %pcrel_lo(.Lpcrel_hi7)
	add	a1, a1, a3
	slli	a2, a2, 9
.Lpcrel_hi8:
	auipc	a3, %pcrel_hi(matrix_a_TMR)
	addi	a3, a3, %pcrel_lo(.Lpcrel_hi8)
	add	a2, a2, a3
	lw	s1, -76(s0)
	lw	a4, -92(s0)
	lw	a5, -108(s0)
	slli	a3, s1, 3
	add	a0, a0, a3
	slli	a3, a4, 3
	add	a1, a1, a3
	slli	a3, a5, 3
	add	a2, a2, a3
	ld	a0, 0(a0)
	ld	s3, 0(a1)
	ld	s2, 0(a2)
	slli	s1, s1, 9
.Lpcrel_hi9:
	auipc	a1, %pcrel_hi(matrix_b)
	addi	a1, a1, %pcrel_lo(.Lpcrel_hi9)
	add	a1, a1, s1
	slli	a4, a4, 9
.Lpcrel_hi10:
	auipc	a2, %pcrel_hi(matrix_b_DWC)
	addi	a2, a2, %pcrel_lo(.Lpcrel_hi10)
	add	a2, a2, a4
	slli	a5, a5, 9
.Lpcrel_hi11:
	auipc	a3, %pcrel_hi(matrix_b_TMR)
	addi	a3, a3, %pcrel_lo(.Lpcrel_hi11)
	add	a3, a3, a5
	lw	a4, -72(s0)
	lw	a5, -88(s0)
	lw	s1, -104(s0)
	slli	a4, a4, 3
	add	a1, a1, a4
	slli	a5, a5, 3
	add	s4, a2, a5
	slli	s1, s1, 3
	add	s5, a3, s1
	ld	a1, 0(a1)
	call	__muldi3@plt
	lw	a1, -80(s0)
	add	s1, a1, a0
	addw	s6, a1, a0
	ld	a1, 0(s4)
	mv	a0, s3
	call	__muldi3@plt
	lw	a1, -96(s0)
	addw	s3, a1, a0
	ld	a1, 0(s5)
	mv	a0, s2
	call	__muldi3@plt
	lw	a2, -112(s0)
	add	a1, a2, a0
	addw	a2, a2, a0
	xor	a0, s6, s3
	seqz	a0, a0
	xor	a2, s6, a2
	seqz	a2, a2
	and	a2, a2, a0
	bnez	a2, .LBB0_36
	j	.LBB0_35
.LBB0_35:                               # %errorHandler.testing140
                                        #   in Loop: Header=BB0_13 Depth=3
.Lpcrel_hi12:
	auipc	a2, %pcrel_hi(TMR_ERROR_CNT)
	addi	a2, a2, %pcrel_lo(.Lpcrel_hi12)
	lw	a3, 0(a2)
	addiw	a3, a3, 1
	sw	a3, 0(a2)
	j	.LBB0_36
.LBB0_36:                               # %testing.cont143
                                        #   in Loop: Header=BB0_13 Depth=3
	andi	a0, a0, 1
	bnez	a0, .LBB0_38
# %bb.37:                               # %testing.cont143
                                        #   in Loop: Header=BB0_13 Depth=3
	mv	s1, a1
.LBB0_38:                               # %testing.cont143
                                        #   in Loop: Header=BB0_13 Depth=3
	sw	s1, -80(s0)
	sw	s1, -96(s0)
	sw	s1, -112(s0)
	j	.LBB0_39
.LBB0_39:                               #   in Loop: Header=BB0_13 Depth=3
	j	.LBB0_40
.LBB0_40:                               #   in Loop: Header=BB0_13 Depth=3
	lw	a3, -76(s0)
	addiw	a0, a3, 1
	lw	a2, -92(s0)
	lw	a4, -108(s0)
	addiw	a1, a4, 1
	xor	a2, a2, a3
	seqz	a2, a2
	xor	a3, a3, a4
	seqz	a3, a3
	and	a3, a3, a2
	bnez	a3, .LBB0_42
	j	.LBB0_41
.LBB0_41:                               # %errorHandler.testing148
                                        #   in Loop: Header=BB0_13 Depth=3
.Lpcrel_hi13:
	auipc	a3, %pcrel_hi(TMR_ERROR_CNT)
	addi	a3, a3, %pcrel_lo(.Lpcrel_hi13)
	lw	a4, 0(a3)
	addiw	a4, a4, 1
	sw	a4, 0(a3)
	j	.LBB0_42
.LBB0_42:                               # %testing.cont151
                                        #   in Loop: Header=BB0_13 Depth=3
	andi	a2, a2, 1
	bnez	a2, .LBB0_44
# %bb.43:                               # %testing.cont151
                                        #   in Loop: Header=BB0_13 Depth=3
	mv	a0, a1
.LBB0_44:                               # %testing.cont151
                                        #   in Loop: Header=BB0_13 Depth=3
	sw	a0, -76(s0)
	sw	a0, -92(s0)
	sw	a0, -108(s0)
	j	.LBB0_13
.LBB0_45:                               #   in Loop: Header=BB0_7 Depth=2
	lw	a0, -80(s0)
	lw	a2, -68(s0)
	lw	a7, -96(s0)
	lw	a3, -84(s0)
	lw	a6, -112(s0)
	lw	a4, -100(s0)
	slli	a2, a2, 9
.Lpcrel_hi14:
	auipc	s1, %pcrel_hi(matrix_c)
	addi	s1, s1, %pcrel_lo(.Lpcrel_hi14)
	add	a2, a2, s1
	slli	a3, a3, 9
.Lpcrel_hi15:
	auipc	s1, %pcrel_hi(matrix_c_DWC)
	addi	s1, s1, %pcrel_lo(.Lpcrel_hi15)
	add	a3, a3, s1
	slli	a4, a4, 9
.Lpcrel_hi16:
	auipc	s1, %pcrel_hi(matrix_c_TMR)
	addi	s1, s1, %pcrel_lo(.Lpcrel_hi16)
	add	a4, a4, s1
	lw	s1, -72(s0)
	lw	a1, -88(s0)
	lw	a5, -104(s0)
	slli	s1, s1, 3
	add	a2, a2, s1
	slli	a1, a1, 3
	add	a3, a3, a1
	slli	a5, a5, 3
	add	a4, a4, a5
	xor	a1, a0, a7
	seqz	a5, a1
	xor	a1, a0, a6
	seqz	a1, a1
	and	a1, a1, a5
	bnez	a1, .LBB0_47
	j	.LBB0_46
.LBB0_46:                               # %errorHandler.testing156
                                        #   in Loop: Header=BB0_7 Depth=2
.Lpcrel_hi17:
	auipc	a1, %pcrel_hi(TMR_ERROR_CNT)
	addi	a1, a1, %pcrel_lo(.Lpcrel_hi17)
	lw	s1, 0(a1)
	addiw	s1, s1, 1
	sw	s1, 0(a1)
	j	.LBB0_47
.LBB0_47:                               # %testing.cont159
                                        #   in Loop: Header=BB0_7 Depth=2
	andi	a5, a5, 1
	bnez	a5, .LBB0_49
# %bb.48:                               # %testing.cont159
                                        #   in Loop: Header=BB0_7 Depth=2
	mv	a0, a6
.LBB0_49:                               # %testing.cont159
                                        #   in Loop: Header=BB0_7 Depth=2
	sd	a0, 0(a2)
	sd	a0, 0(a3)
	sd	a0, 0(a4)
	j	.LBB0_50
.LBB0_50:                               #   in Loop: Header=BB0_7 Depth=2
	lw	a3, -72(s0)
	addiw	a0, a3, 1
	lw	a2, -88(s0)
	lw	a4, -104(s0)
	addiw	a1, a4, 1
	xor	a2, a2, a3
	seqz	a2, a2
	xor	a3, a3, a4
	seqz	a3, a3
	and	a3, a3, a2
	bnez	a3, .LBB0_52
	j	.LBB0_51
.LBB0_51:                               # %errorHandler.testing164
                                        #   in Loop: Header=BB0_7 Depth=2
.Lpcrel_hi18:
	auipc	a3, %pcrel_hi(TMR_ERROR_CNT)
	addi	a3, a3, %pcrel_lo(.Lpcrel_hi18)
	lw	a4, 0(a3)
	addiw	a4, a4, 1
	sw	a4, 0(a3)
	j	.LBB0_52
.LBB0_52:                               # %testing.cont167
                                        #   in Loop: Header=BB0_7 Depth=2
	andi	a2, a2, 1
	bnez	a2, .LBB0_54
# %bb.53:                               # %testing.cont167
                                        #   in Loop: Header=BB0_7 Depth=2
	mv	a0, a1
.LBB0_54:                               # %testing.cont167
                                        #   in Loop: Header=BB0_7 Depth=2
	sw	a0, -72(s0)
	sw	a0, -88(s0)
	sw	a0, -104(s0)
	j	.LBB0_7
.LBB0_55:                               #   in Loop: Header=BB0_1 Depth=1
	j	.LBB0_56
.LBB0_56:                               #   in Loop: Header=BB0_1 Depth=1
	lw	a3, -68(s0)
	addiw	a0, a3, 1
	lw	a2, -84(s0)
	lw	a4, -100(s0)
	addiw	a1, a4, 1
	xor	a2, a2, a3
	seqz	a2, a2
	xor	a3, a3, a4
	seqz	a3, a3
	and	a3, a3, a2
	bnez	a3, .LBB0_58
	j	.LBB0_57
.LBB0_57:                               # %errorHandler.testing172
                                        #   in Loop: Header=BB0_1 Depth=1
.Lpcrel_hi19:
	auipc	a3, %pcrel_hi(TMR_ERROR_CNT)
	addi	a3, a3, %pcrel_lo(.Lpcrel_hi19)
	lw	a4, 0(a3)
	addiw	a4, a4, 1
	sw	a4, 0(a3)
	j	.LBB0_58
.LBB0_58:                               # %testing.cont175
                                        #   in Loop: Header=BB0_1 Depth=1
	andi	a2, a2, 1
	bnez	a2, .LBB0_60
# %bb.59:                               # %testing.cont175
                                        #   in Loop: Header=BB0_1 Depth=1
	mv	a0, a1
.LBB0_60:                               # %testing.cont175
                                        #   in Loop: Header=BB0_1 Depth=1
	sw	a0, -68(s0)
	sw	a0, -84(s0)
	sw	a0, -100(s0)
	j	.LBB0_1
.LBB0_61:
	ld	ra, 104(sp)                     # 8-byte Folded Reload
	ld	s0, 96(sp)                      # 8-byte Folded Reload
	ld	s1, 88(sp)                      # 8-byte Folded Reload
	ld	s2, 80(sp)                      # 8-byte Folded Reload
	ld	s3, 72(sp)                      # 8-byte Folded Reload
	ld	s4, 64(sp)                      # 8-byte Folded Reload
	ld	s5, 56(sp)                      # 8-byte Folded Reload
	ld	s6, 48(sp)                      # 8-byte Folded Reload
	addi	sp, sp, 112
	ret
.Lfunc_end0:
	.size	testing, .Lfunc_end0-testing
                                        # -- End function
	.globl	mm                              # -- Begin function mm
	.p2align	1
	.type	mm,@function
mm:                                     # @mm
# %bb.0:
	addi	sp, sp, -16
	sd	ra, 8(sp)                       # 8-byte Folded Spill
	sd	s0, 0(sp)                       # 8-byte Folded Spill
	addi	s0, sp, 16
	call	testing
	ld	ra, 8(sp)                       # 8-byte Folded Reload
	ld	s0, 0(sp)                       # 8-byte Folded Reload
	addi	sp, sp, 16
	ret
.Lfunc_end1:
	.size	mm, .Lfunc_end1-mm
                                        # -- End function
	.type	matrix_a_DWC,@object            # @matrix_a_DWC
	#.local	matrix_a_DWC
	.comm	matrix_a_DWC,32768,8
	.type	matrix_a_TMR,@object            # @matrix_a_TMR
	#.local	matrix_a_TMR
	.comm	matrix_a_TMR,32768,8
	.type	matrix_a,@object                # @matrix_a
	#.local	matrix_a
	.comm	matrix_a,32768,8
	.type	matrix_b_DWC,@object            # @matrix_b_DWC
	#.local	matrix_b_DWC
	.comm	matrix_b_DWC,32768,8
	.type	matrix_b_TMR,@object            # @matrix_b_TMR
	#.local	matrix_b_TMR
	.comm	matrix_b_TMR,32768,8
	.type	matrix_b,@object                # @matrix_b
	#.local	matrix_b
	.comm	matrix_b,32768,8
	.type	matrix_c_DWC,@object            # @matrix_c_DWC
	#.local	matrix_c_DWC
	.comm	matrix_c_DWC,32768,8
	.type	matrix_c_TMR,@object            # @matrix_c_TMR
	#.local	matrix_c_TMR
	.comm	matrix_c_TMR,32768,8
	.type	matrix_c,@object                # @matrix_c
	#.local	matrix_c
	.comm	matrix_c,32768,8
	.type	TMR_ERROR_CNT,@object           # @TMR_ERROR_CNT
	.section	.sbss,"aw",@nobits
	.globl	TMR_ERROR_CNT
	.p2align	2
TMR_ERROR_CNT:
	.word	0                               # 0x0
	.size	TMR_ERROR_CNT, 4

	.ident	"Debian clang version 16.0.6 (15~deb12u1)"
	.section	".note.GNU-stack","",@progbits

    add $t0, $imm1, $imm1, $imm1, 0, 0 #i_th row of A
    add $t1, $imm1, $imm1, $imm1, 0, 0 #j_th column of B
    add $t2, $imm1, $imm1, $imm1, 0, 0 #k_th column of A, k_th row of B
    sll $a2, $imm1, $imm2, $imm2, 1, 2 #set dimension of matrix to 4
	beq $zero, $zero, $zero, $imm2,0, MUL_A_ROW_B_COL


	
INC_A_ROW:
    sub $t1, $t1, $a2, $imm2, 0, 0 #j = 0
    add $t0, $t0, $imm1, $imm2, 1, 0 #i = i+1
    bge $zero, $t0, $a2, $imm2, 0, FINISHED #if(i>=4) jump to FINISHED
	beq $zero, $zero, $zero, $imm2,0, MUL_A_ROW_B_COL #
	
	
INC_B_COL:
    sub $t2, $t2, $a2, $imm2, 0, 0 #k = 0
    add $t1, $t1, $imm1, $imm2, 1, 0 #j = j+1
    bge $zero, $t1, $a2, $imm2, 0, INC_A_ROW #if(j>=4) jump to INC_A_ROW - next row


MUL_A_ROW_B_COL:
    mac $a0, $a2, $t0, $imm2, 0, 0x100 #$a0 = (4i+0x100)
    mac $a1, $a2, $t2, $imm2, 0, 0x110 #$a1 = (4k+0x110)
    mac $s0, $a2, $t0, $imm2, 0, 0x120 #$s0 = (4i+0x120)
    add $s1, $s0, $imm1, $imm1, 0, 0 #$s1 = (4i+0x120)
    lw $a0, $a0, $t2, $imm1, 0, 0 #$a0 = A[i][k]
    lw $a1, $a1, $t1, $imm1, 0, 0 #$a1 = B[k][j]
    lw $s0, $s0, $t1, $imm1, 0, 0 #$s0 = C[i][j]
    mac $s0, $a0, $a1, $s0, 0, 0 #$s0 = C[i][j] + A[i][k] x B[k][j]
    sw $s0, $s1, $t1, $imm1, 0, 0 #C[i][j] = C[i][j] + A[i][k] x B[k][j]
    add $t2, $t2, $imm1, $imm2, 0, 1 #k = k+1
    bge $zero, $t2, $a2, $imm2, 0, INC_B_COL #if(k>=4) jump to INC_B_COL - next column
    beq $zero, $zero, $zero, $imm2,0, MUL_A_ROW_B_COL #

FINISHED:
    halt $imm1, $imm1, $imm2, $imm2, 0, 0 #

.word 0x100 1
.word 0x101 2
.word 0x102 3
.word 0x103 9
.word 0x104 0xB
.word 0x105 17
.word 0x106 17
.word 0x107 0xA
.word 0x108 0
.word 0x109 4
.word 0x10A 5
.word 0x10B 3
.word 0x10C 9
.word 0x10D 1
.word 0x10E 0
.word 0x10F 10
.word 0x110 1
.word 0x111 1
.word 0x112 1
.word 0x113 1
.word 0x114 8
.word 0x115 8
.word 0x116 8
.word 0x117 8
.word 0x118 0
.word 0x119 2
.word 0x11A 0
.word 0x11B 2
.word 0x11C 3
.word 0x11D 4
.word 0x11E 0x3
.word 0x11F 0
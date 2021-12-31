
.word 0x100 15
.word 0x101 14
.word 0x102 13
.word 0x103 12
.word 0x104 11
.word 0x105 10
.word 0x106 9
.word 0x107 8
.word 0x108 7
.word 0x109 6
.word 0x10A 5
.word 0x10B 4
.word 0x10C 3
.word 0x10D 2
.word 0x10E 1
.word 0x10F 0

.word 0x110 0
.word 0x111 -5
.word 0x112 -3
.word 0x113 1
.word 0x114 0
.word 0x115 26
.word 0x116 9
.word 0x117 -26
.word 0x118 0
.word 0x119 24
.word 0x11A -2
.word 0x11B 6
.word 0x11C 0
.word 0x11D 23
.word 0x11E -3
.word 0x11f 10

# expected: 
# [ 0 877 19 -151 ]
# [ 0 605 15 -115 ]
# [ 0 333 11 -79 ]
# [ 0 61 7 -43 ]

        sll $sp, $imm1, $imm2, $zero, 1, 11                 # set $sp = 1 << 11 = 2048
		add $sp, $sp, $imm2, $zero, 0, -5  	                # adjust stack for 5 items
		sw $s0, $sp, $imm2, $zero, 0, 4 				    # store value of s0 in stack
		sw $s1, $sp, $imm2, $zero, 0, 3			            # store value of s1 in stack
		sw $s2, $sp, $imm2, $zero, 0, 2				        # store value of s2 in stack
        sw $a0, $sp, $imm2, $zero, 0, 1 			        # store value of a0 in stack
        sw $a1, $sp, $imm2, $zero, 0, 0				        # store value of a1 in stack
		
		add $s0, $imm2, $zero, $zero, 0, 0x100              # s0 = A[0] : first matrix
		add $s1, $imm2, $zero, $zero, 0, 0x110              # s1 = B[0] : second matrix
		add $s2, $imm2, $zero, $zero, 0, 0x120              # s2 = C[0] : result matrix
        add $a0, $zero, $zero, $zero, 0, 0                  # a0 = 0 : for1 counter
        add $a1, $zero, $zero, $zero, 0, 0                  # a1 = 0 : for2 counter
		
for1:	
        add $a0, $a0, $imm2, $zero, 0, 1                    # a0++
        beq $zero, $a0, $imm1, $imm2, 1, for2               # if a0 == 1 jump to for2 : first iteration
        bgt $zero, $a2, $imm1, $imm2, 4, return             # if a0 > 4 jump to return
        add $a1, $zero, $zero, $zero, 0, 0                  # a1 = 0 
        add $s0, $s0, $imm2, $zero, 0, 4                    # s0 = A[s0 + 4] : next row
        add $s1, $imm2, $zero, $zero, 0, 0x110              # s1 = B[0] 
		
for2:	  
        add $t0, $zero, $zero, $zero, 0, 0                  # t0 = 0
        lw $t1, $s0, $imm2, $zero, 0, 0                     # t1 = A[s0]
        lw $t2, $s1, $imm2, $zero, 0, 0                     # t2 = B[s1]				
		mac $t0, $t1, $t2, $t0, 0, 0   			            # t0 = t1*t2 + $t0

        lw $t1, $s0, $imm2, $zero, 0, 1                     # t1 = A[s0 + 1]
        lw $t2, $s1, $imm2, $zero, 0, 4                     # t2 = B[s1 + 4]				
		mac $t0, $t1, $t2, $t0, 0, 0   			            # t0 = t1*t2 + $t0

        lw $t1, $s0, $imm2, $zero, 0, 2                     # t1 = A[s0 + 2]
        lw $t2, $s1, $imm2, $zero, 0, 8                     # t2 = B[s1 + 8]				
		mac $t0, $t1, $t2, $t0, 0, 0   			            # t0 = t1*t2 + $t0

        lw $t1, $s0, $imm2, $zero, 0, 3                     # t1 = A[s0 + 3]
        lw $t2, $s1, $imm2, $zero, 0, 12                    # t2 = B[s1 + 12]				
		mac $t0, $t1, $t2, $t0, 0, 0   			            # t0 = t1*t2 + $t0

		sw $t0, $s2, $zero, $zero, 0, 0                     # C[0] = t0
        
        add $s1, $s1, $imm2, $zero, 0, 1                    # s1 = B[s1 + 1] : next column
        add $s2, $s2, $imm2, $zero, 0, 1                    # s2 = C[s2 + 1] : next element
        add $a1, $a1, $imm2, $zero, 0, 1                    # a1++
		
        beq $zero, $a1, $imm1, $imm2, 4, for1               # if a1 == 4 jump to for1
        beq $zero, $zero, $zero, $imm2, 0, for2             # else jump to for 2
				
return:
        lw $a1, $sp, $imm2, $zero, 0, 0                     # restore $a1
        lw $a0, $sp, $imm2, $zero, 0, 1                     # restore $a0
        lw $s2, $sp, $imm2, $zero, 0, 2                     # restore $s2
        lw $s1, $sp, $imm2, $zero, 0, 3                     # restore $s1
        lw $s0, $sp, $imm2, $zero, 0, 4                     # restore $s0
        add $sp, $sp, $imm2, $zero, 0, 5                    # pop 5 items from stack
		halt $zero, $zero, $zero, $zero, 0, 0               # halt

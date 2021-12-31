.word 0x100 9
.word 0x101 5

# expected: 126 

os:
    sll $sp, $imm1, $imm2, $zero, 1, 11             # set $sp = 1 << 11 = 2048
    
main:
    lw $a0, $zero, $imm2, $zero, 0, 0x100           # get n from address 0x100
    lw $a1, $zero, $imm2, $zero, 0, 0x101           # get k from address 0x101
    jal $ra, $zero, $zero, $imm2, 0, binom          # calc $v0 = binom(n, k)
    sw $v0, $zero, $imm2, $zero, 0, 0x102           # store binom(n, k) in 0x102
    halt $zero, $zero, $zero, $zero, 0, 0           # halt

binom:
    add $sp, $sp, $imm2, $zero, 0, -4               # adjust stack for 4 items
    sw $s0, $sp, $imm2, $zero, 0, 3                 # save $s0
    sw $ra, $sp, $imm2, $zero, 0, 2                 # save return address
    sw $a1, $sp, $imm2, $zero, 0, 1                 # save argument 1
    sw $a0, $sp, $imm2, $zero, 0, 0                 # save argument 2
    beq $zero, $a1, $imm1, $imm2, 0, stop           # jump to stop if k == 0
    beq $zero, $a0, $a1, $imm2, 0, stop             # jump to stop if k == n
    beq $zero, $zero, $zero, $imm2, 0, binom_rec    # jump to binom_rec;

return:
    lw $a0, $sp, $imm2, $zero, 0, 0                 # restore $a0
    lw $a1, $sp, $imm2, $zero, 0, 1                 # restore $a1
    lw $ra, $sp, $imm2, $zero, 0, 2                 # restore $ra
    lw $s0, $sp, $imm2, $zero, 0, 3                 # restore $s0
    add $sp, $sp, $imm2, $zero, 0, 4                # pop 4 items from stack
    beq $zero, $zero, $zero, $ra, 0, 0              # return

stop:
    add $v0, $zero, $zero, $imm2, 0, 1              # $v0 = 1
    beq $zero, $zero, $zero, $imm2, 0, return       # return

binom_rec:
    add $a0, $a0, $zero, $imm2, 0, -1               # $a0 = n-1
    add $a1, $a1, $zero, $imm2, 0, -1               # $a1 = k-1
    jal $ra, $zero, $zero, $imm2, 0, binom          # calc $v0 = binom(n-1, k-1)
    add $s0, $v0, $zero, $zero, 0, 0                # $s0 = $v0 = binom(n-1, k-1)
    add $a1, $a1, $zero, $imm2, 0, 1                # $a1 = k
    jal $ra, $zero, $zero, $imm2, 0, binom          # calc $v0 = binom(n-1, k)
    add $v0, $v0, $s0, $zero, 0, 0                  # $v0 = $s0 + $v0(=binom(n-1, k))
    beq $zero, $zero, $zero, $imm2, 0, return       # return

#   int binom(n, k)                                 # label = binom
#   {
#       if (k == 0 || n == k)
#           return 1;                               # label = stop
#       return binom(n-1, k-1) + binom(n-1, k);     # label = binom_rec
#   }                                               # label = return
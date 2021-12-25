os:
    sll $sp, $imm1, $imm2, $zero, 1, 11             # set $sp = 1 << 11 = 2048

main: 
    lw $a0, $zero, $imm2, 0, 0x100                  # load radius from 0x100 to $a0
    mac $s2, $a0, $a0, $zero, 0, 0                  # $s2 = radiusSquerd = radius * radius
    sll $s0, $imm1, $imm2, $zero, 1, 8              # $s0 = MonitorSize = 256
    add $t0, $zero, $zero, $zero, 0 ,0              # row = $t0 = 0

ForLoopRow:
    bge $zero, $t0, $s0, $imm2, 0, Return           # if ( row >= MonitorSize{=256} ) jump to Return
    add $t1, $zero, $zero, $zero, 0 ,0              # col = $t1 = 0

ForLoopCol: 
    bge $zero, $t1, $s0, $imm2, 0, EndLoopRow       # if ( col >= MonitorSize{=256} ) jump to EndLoopRow
    sub $s1, $t0, $imm1, $zero, 128, 0              # distFromCenter = row - 128
    mac $s1, $s1, $s1, $zero, 0, 0                  # distFromCenter = ( row - 128 ) * ( row - 128 )
    sub $t2, $t1, $imm1, $zero, 128, 0              # $t2 = col - 128
    mac $t2, $t2, $t2, $zero, 0, 0                  # $t2 = ( col - 128 ) * ( col - 128 )
    add $s1, $t2, $s1, $zero, 0, 0                  # distFromCenter = ( col - 128 ) * ( col - 128 ) + ( row - 128 ) * ( row - 128 )
    ble $zero, $s1, $s2, $imm2, 0, PaintWhite       # if ( distFromCenter <= radiusSquerd ) jump to PaintWhite

EndLoopCol:
    add $t1, $t1, $imm1, $zero, 1, 0                # col++
    beq $zero, $zero, $zero, $imm2, 0, ForLoopCol   # jump to ForLoopCol

EndLoopRow:
    add $t0, $imm, $zero, $zero, 1, 0               # row++ 
    beq $zero, $zero, $zero, $imm2, 0, ForLoopRow   # jump to ForLoopRow 

PaintWhite: 
    sll $t2, $t0, $imm1, $zero, 8, 0                # $t2 = row << 8
    add $t2, $t2, $t1, $zero, 0, 0                  # $t2 = row << 8 + col { offset in monitor }
    out $zero, $imm1, $zero, $t2, 20, 0             # IORegister[ monitoraddr = 20 ] = $t2 = offset monitor[ row ][ col ]
    out $zero, $imm1, $zero, $imm, 21, 255          # IORegister[ monitordata = 21 ] = 255 { White }
    out $zero, $imm1, $imm, $imm2, 22, 1            # IORegister[ monitorcmd = 22 ] = 1 { Write command }
    beq $zero, $zero, $zero, $imm2, 0, EndLoopCol   # jump to EndLoopCol 

Return: 
	halt $zero, $zero, $zero, $zero, 0, 0           # halt


# $a0 = radius
# $s0 = MonitorSize
# $s1 = distFromCenter
# $s2 = radius * radius (abuse for having another tmp param)
# $t0 = row
# $t1 = col
# $t2 = another tmp reg


# void drawCircle() {
#     int radius = lw(0x100);
#     int radiusSquered = radius * radius;
#     int MonitorSize = 256;
#     for ( int row = 0; row < 256; row++ ) {
#         for ( int col = 0; col < 256; col++ ) {
#             int distFromCenter = ( col - 128 ) * ( col - 128 ) + ( row - 128 ) * ( row - 128 );
#             if ( distFromCenter <= radiusSquered ) {
#                 monitor[row][col] = 255;
#             }
#         }
#     }
# }

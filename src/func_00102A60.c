// Hybrid: branches/j as .word, jal with extern decls
extern void func_001029E8(int, int, int, int);
extern void func_00102A90(int, int, int, int);

asm void func_00102A60(void) {
    mtc1       $zero, $f0
    c.lt.s     $f12, $f0
    lui        $at, (0x3FC90FDB >> 16)
    .word 0x34210fdb
    mtc1       $at, $f0
    .word 0x45000004
    nop
    add.s      $f12, $f0, $f12
    j         func_00102A90
    addiu     $a3, $zero, 0x1
    sub.s      $f12, $f0, $f12
    daddu      $a3, $zero, $zero
    mfc1       $8, $f12
    qmtc2.ni   $8, $vf6
    daddu      $a2, $ra, $zero
    jal        func_001029E8
    nop
    daddu      $ra, $a2, $zero
    .word 0x4be62b3c  /* was: vmove.xyzw $vf6, $vf5 */
    .word 0x4be72b3c  /* was: vmove.xyzw $vf7, $vf5 */
    .word 0x4be9033c  /* was: vmove.xyzw $vf9, $vf0 */
    .word 0x4bc94a6c  /* was: vsub.xyz   $vf9, $vf9, $vf9 */
    .word 0x4be84b3d  /* was: vmr32.xyzw $vf8, $vf9 */
    .word 0x4a64212c  /* was: vsub.zw    $vf4, $vf4, $vf4 */
    .word 0x4a842980  /* was: vaddx.y    $vf6, $vf5, $vf4x */
    .word 0x4b042981  /* was: vaddy.x    $vf6, $vf5, $vf4y */
    .word 0x4b0429c4  /* was: vsubx.x    $vf7, $vf5, $vf4x */
    .word 0x4a8429c1  /* was: vaddy.y    $vf7, $vf5, $vf4y */
    addiu      $a3, $zero, 0x4
    lqc2       $vf4, 0x0($a1)
    .word 0x4be431bc  /* was: vmulax.xyzw ACC, $vf6, $vf4x */
    .word 0x4be438bd  /* was: vmadday.xyzw ACC, $vf7, $vf4y */
    .word 0x4be440be  /* was: vmaddaz.xyzw ACC, $vf8, $vf4z */
    .word 0x4be4494b  /* was: vmaddw.xyzw $vf5, $vf9, $vf4w */
    sqc2       $vf5, 0x0($a0)
    addi       $a3, $a3, -0x1 /* handwritten instruction */
    addi       $a1, $a1, 0x10 /* handwritten instruction */
    .word 0x1407fff7
    addi      $a0, $a0, 0x10 /* handwritten instruction */
    jr         $ra
    nop
}

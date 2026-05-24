// Hybrid: branches/j as .word, jal with extern decls
extern void func_001CF9D8(int, int, int, int);

asm void func_001CF970(void) {
    lqc2       $vf10, 0x40($a1)
    lqc2       $vf11, 0x40($a2)
    lqc2       $vf21, 0x0($a1)
    lqc2       $vf22, 0x10($a1)
    lqc2       $vf23, 0x20($a1)
    lqc2       $vf24, 0x30($a1)
    lqc2       $vf25, 0x0($a2)
    lqc2       $vf26, 0x10($a2)
    lqc2       $vf27, 0x20($a2)
    lqc2       $vf28, 0x30($a2)
    .word 0x4bea5bec  /* was: vsub.xyzw  $vf15, $vf11, $vf10 */
    .word 0x4bf5ce6c  /* was: vsub.xyzw  $vf25, $vf25, $vf21 */
    .word 0x4bf6d6ac  /* was: vsub.xyzw  $vf26, $vf26, $vf22 */
    .word 0x4bf7deec  /* was: vsub.xyzw  $vf27, $vf27, $vf23 */
    .word 0x4bf8e72c  /* was: vsub.xyzw  $vf28, $vf28, $vf24 */
    mfc1       $8, $f12
    nop
    qmtc2.ni   $8, $vf9
    .word 0x4a295318  /* was: vmulx.w    $vf12, $vf10, $vf9x */
    .word 0x4a295b58  /* was: vmulx.w    $vf13, $vf11, $vf9x */
    .word 0x4bec5407  /* was: vsubw.xyzw $vf16, $vf10, $vf12w */
    .word 0x4bed5c47  /* was: vsubw.xyzw $vf17, $vf11, $vf13w */
    .word 0x4bf081fd  /* was: vabs.xyzw  $vf16, $vf16 */
    .word 0x4bf189fd  /* was: vabs.xyzw  $vf17, $vf17 */
    qmfc2.ni   $9, $vf16
    qmfc2.ni   $10, $vf17
    .word 0x10e00007
    nop
    prot3w     $9, $9
    prot3w     $10, $10
    addi       $a3, $a3, -0x1 /* handwritten instruction */
    nop
    j         func_001CF9D8
    nop
    qmtc2.ni   $9, $vf16
    qmtc2.ni   $10, $vf17
    .word 0x4bd08ca8  /* was: vadd.xyz   $vf18, $vf17, $vf16 */
    .word 0x4a1283bc  /* was: vdiv       Q, $vf16x, $vf18x */
    .word 0x4a0003bf  /* was: vwaitq */
    .word 0x4be00520  /* was: vaddq.xyzw $vf20, $vf0, Q */
    .word 0x4b14a1fd  /* was: vabs.x     $vf20, $vf20 */
    .word 0x4bf47bd8  /* was: vmulx.xyzw $vf15, $vf15, $vf20x */
    .word 0x4bf4ce58  /* was: vmulx.xyzw $vf25, $vf25, $vf20x */
    .word 0x4bf4d698  /* was: vmulx.xyzw $vf26, $vf26, $vf20x */
    .word 0x4bf4ded8  /* was: vmulx.xyzw $vf27, $vf27, $vf20x */
    .word 0x4bf4e718  /* was: vmulx.xyzw $vf28, $vf28, $vf20x */
    .word 0x4bef54e8  /* was: vadd.xyzw  $vf19, $vf10, $vf15 */
    .word 0x4bf5ce68  /* was: vadd.xyzw  $vf25, $vf25, $vf21 */
    .word 0x4bf6d6a8  /* was: vadd.xyzw  $vf26, $vf26, $vf22 */
    .word 0x4bf7dee8  /* was: vadd.xyzw  $vf27, $vf27, $vf23 */
    .word 0x4bf8e728  /* was: vadd.xyzw  $vf28, $vf28, $vf24 */
    sqc2       $vf25, 0x0($a0)
    sqc2       $vf26, 0x10($a0)
    sqc2       $vf27, 0x20($a0)
    sqc2       $vf28, 0x30($a0)
    jr         $ra
    sqc2      $vf19, 0x40($a0)
}

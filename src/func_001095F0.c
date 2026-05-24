// Hybrid-strict: MMI+lui-literal as .word, jal with extern decls
extern void func_0010B160(int, int, int, int);

asm void func_001095F0(void) {
    .word 0x3c030001
    nop
    di /* handwritten instruction */
    sync.p
    mfc0       $v0, $12 /* handwritten instruction */
    and        $v0, $v0, $v1
    nop
    .word 0x1440fffa
    nop
    .word 0x3c091000
    .word 0x3c030001
    ori        $9, $9, (0x1000F520 & 0xFFFF)
    .word 0x3c081000
    .word 0x8d220000
    ori        $8, $8, (0x1000F590 & 0xFFFF)
    .word 0x3c051000
    .word 0x3c07ffff
    or         $v0, $v0, $v1
    ori        $a1, $a1, (0x1000B000 & 0xFFFF)
    .word 0xad020000
    ori        $a3, $a3, (0xFFFFFEFF & 0xFFFF)
    .word 0x3c041000
    .word 0x3c06fffe
    .word 0x8ca30000
    ori        $a0, $a0, (0x1000B400 & 0xFFFF)
    ori        $a2, $a2, (0xFFFEFFFF & 0xFFFF)
    and        $v1, $v1, $a3
    .word 0xaca30000
    .word 0x8c820000
    and        $v0, $v0, $a3
    .word 0xac820000
    .word 0x8d230000
    and        $v1, $v1, $a2
    .word 0xad030000
    ei /* handwritten instruction */
    .word 0x3c021000
    .word 0x3c031000
    ori        $v0, $v0, (0x1000B020 & 0xFFFF)
    ori        $v1, $v1, (0x1000B420 & 0xFFFF)
    .word 0xac400000
    j         func_0010B160
    .word 0xac600000
}

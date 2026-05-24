// Hybrid: branches/j as .word, jal with extern decls
extern void func_001D7FA0(int, int, int, int);

asm void func_001D8100(void) {
    lui        $v0, (0xBD4CCCCD >> 16)
    ori        $v0, $v0, (0xBD4CCCCD & 0xFFFF)
    mtc1       $v0, $f13
    lui        $v0, (0x3F733333 >> 16)
    ori        $v0, $v0, (0x3F733333 & 0xFFFF)
    mtc1       $v0, $f12
    nop
    j         func_001D7FA0
    paddub    $a2, $zero, $zero
}

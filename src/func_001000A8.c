// All-word: everything as .word except jal/j-external (for R_MIPS_26 relocs)
extern void Exit(int, int, int, int);

asm void func_001000A8(void) {
    j         Exit
    .word 0x00002025
}

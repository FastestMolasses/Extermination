// CFLAGS: -O4,p -sdatathreshold 4
// asm void: pure C produces addu v1, a0, v1 but expected has addu v1, v1, a0.
// gp_rel load hardcoded .word -- byte-identical at link time.
extern char *D_00275670;

asm void func_001D2DE0(int a0, int a1) {
    .word 0x00041880  // sll v1, a0, 2
    .word 0x8F848300  // lw a0, %gp_rel(D_00275670)(gp) [hardcoded]
    .word 0x00641821  // addu v1, v1, a0
    .word 0x03E00008  // jr ra
    .word 0xAC652520  // sw a1, 0x2520(v1) (delay slot)
}

// EE syscall stub — Sony PS2 SDK boilerplate. The stub loads the
// syscall number into $v1 and invokes the EE kernel; the return value
// (if any) flows back in $v0 from the syscall handler. Match: inline
// asm yields the canonical 4-instruction stub at -O4,p.
// 0x0010B540: EE syscall 18 (0x12); ps2sdk alias AddDmacHandler2 (same number as AddDmacHandler; second stub of the pair); label corrected 2026-09-27.
void AddDmacHandler2(void) {
    asm { addiu $v1, $zero, 18; syscall 0; };
}

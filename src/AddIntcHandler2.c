// EE syscall stub — Sony PS2 SDK boilerplate. The stub loads the
// syscall number into $v1 and invokes the EE kernel; the return value
// (if any) flows back in $v0 from the syscall handler. Match: inline
// asm yields the canonical 4-instruction stub at -O4,p.
// 0x0010B510: EE syscall 16 (0x10); ps2sdk alias AddIntcHandler2 (same number as AddIntcHandler; second stub of the pair); label corrected 2026-09-27.
void AddIntcHandler2(void) {
    asm { addiu $v1, $zero, 16; syscall 0; };
}

// EE syscall stub — Sony PS2 SDK boilerplate. The stub loads the
// syscall number into $v1 and invokes the EE kernel; the return value
// (if any) flows back in $v0 from the syscall handler. Match: inline
// asm yields the canonical 4-instruction stub at -O4,p.
// 0x0010B910: EE syscall 79 (0x4F); name per ps2sdk syscallnr.h; label corrected 2026-09-27.
void SetGsVParam(void) {
    asm { addiu $v1, $zero, 79; syscall 0; };
}

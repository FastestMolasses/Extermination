// EE syscall stub — Sony PS2 SDK boilerplate. The stub loads the
// syscall number into $v1 and invokes the EE kernel; the return value
// (if any) flows back in $v0 from the syscall handler. Match: inline
// asm yields the canonical 4-instruction stub at -O4,p.
// 0x0010B830: EE syscall 65 (0x41); label corrected 2026-09-27 (was RFU063).
void DeleteSema(void) {
    asm { addiu $v1, $zero, 65; syscall 0; };
}

// EE syscall stub — Sony PS2 SDK boilerplate. The stub loads the
// syscall number into $v1 and invokes the EE kernel; the return value
// (if any) flows back in $v0 from the syscall handler. Match: inline
// asm yields the canonical 4-instruction stub at -O4,p.
// 0x0010B5B0: EE syscall 25 (0x19); name per ps2sdk syscallnr.h; label corrected 2026-09-27.
void _ReleaseAlarm(void) {
    asm { addiu $v1, $zero, 25; syscall 0; };
}

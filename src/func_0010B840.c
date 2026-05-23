// EE syscall stub — Sony PS2 SDK boilerplate. The stub loads the
// syscall number into $v1 and invokes the EE kernel; the return value
// (if any) flows back in $v0 from the syscall handler. Match: inline
// asm yields the canonical 4-instruction stub at -O4,p.
void func_0010B840(void) {
    asm { addiu $v1, $zero, 66; syscall 0; };
}

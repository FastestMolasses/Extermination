extern void ChangeThreadPriority(void);

asm void func_00203980(void) {
    j ChangeThreadPriority
    addiu $a0, $zero, 0x5
}

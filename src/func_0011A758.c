// Tail-call wrapper: addiu $a0,$zero,0x46; daddu $a1..$a3,$zero,$zero; j func_001157F0
extern void func_001157F0(void);

asm void func_0011A758(void) {
    addiu $4, $zero, 0x46
    daddu $5, $zero, $zero
    daddu $6, $zero, $zero
    j func_001157F0
    daddu $7, $zero, $zero
}

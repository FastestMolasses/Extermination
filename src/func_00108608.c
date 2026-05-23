// Tail-call wrapper: set struct fields then jump to func_00108660
extern void func_00108660(void);

asm void func_00108608(void) {
    daddu $2, $4, $zero
    daddu $8, $5, $zero
    addu $3, $6, $7
    sw $8, 0xC($2)
    sw $3, 0x24($2)
    sw $7, 0x28($2)
    daddu $5, $zero, $zero
    sw $8, 0x8($2)
    sd $zero, 0x0($2)
    sw $zero, 0x10($2)
    sd $zero, 0x18($2)
    j func_00108660
    sw $6, 0x20($2)
}

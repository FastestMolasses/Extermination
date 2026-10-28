// Simple nonleaf asm function: newlib-shaped _printf_r(r, fmt, ...) -- spills the
// variadic registers and hands (r, r->_stdout, fmt, ap) to the vfprintf core,
// returning its int result.
struct reent;
struct file;
extern int sub__0000000000000000Inf(struct reent *, struct file *, const char *, char *);

asm int func_00122B18(struct reent *r, const char *fmt, ...) {
    addiu      $sp, $sp, -0x80
    sd         $a2, 0x50($sp)
    sd         $a3, 0x58($sp)
    daddu      $a2, $a1, $zero
    sd         $ra, 0x0($sp)
    addiu      $a3, $sp, 0x50
    sd         $8, 0x60($sp)
    sd         $9, 0x68($sp)
    sd         $10, 0x70($sp)
    sd         $11, 0x78($sp)
    jal        sub__0000000000000000Inf
    lw        $a1, 0x8($a0)
    ld         $ra, 0x0($sp)
    jr         $ra
    addiu     $sp, $sp, 0x80
}

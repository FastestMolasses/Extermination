// NEARMISS func_001C5570  (vram 0x001C5570, 0x108 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.33% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-materialization artifact: target holds a 16-bit-masked zero (andi v0,zero,0xffff) in a GPR and stores it to the two byte fields p+3/p+0x9A; mwcc stores $zero directly. 68/70 instructions match. Not the clean-store delay-slot nop; this is a regalloc/CSE coloring choice -> permuter pass.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern char *func_001AFA90(int);
extern void func_00102948(char *, char *);
extern void func_001C5680(void);
extern void func_001C5760(void);

char *func_001C5570(char *arg0, int arg1, char arg2, int arg3) {
    char *p;

    p = func_001AFA90(0xC);
    if (p != 0) {
        *(short *)(p + 0x2E) = 0;
        *(char *)(p + 3) = 0;
        *(char *)(p + 0x9A) = 0;
        *(char *)(p + 0xD) = arg2;
        *(unsigned short *)(p + 0xE) = 0xFFFF;
        *(short *)(p + 0x56) = 0;
        *(short *)(p + 0x54) = 0;
        func_00102948(p + 0xA0, (char *)arg1);
        func_00102948(p + 0xB0, arg0 + 0xB0);
        func_00102948(p + 0xC0, arg0 + 0xC0);
        *(char *)(p + 0xA) = 0;
        switch (arg3) {
        case 0:
            *(void (**)(void))(p + 0x10) = func_001C5760;
            break;
        case 1:
            *(void (**)(void))(p + 0x10) = func_001C5680;
            break;
        case 2:
            *(char *)(p + 0xA) = 1;
            *(void (**)(void))(p + 0x10) = func_001C5760;
            break;
        }
    }
    return p;
}

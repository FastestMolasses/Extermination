// NEARMISS func_001284E0  (vram 0x001284E0, 0x114 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.86% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation coloring (permuter-class): the < 0xB guard's slti destination is $v0 (overwriting the loaded global) where the target uses $at to preserve $v0; single-instruction residual, unaffected by structure/flags. Not the clean-store nop.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern char *func_001AFA90(int);
extern void func_0012A5D0();

int func_001284E0(char *arg0, char *arg1, int arg2, char *arg3) {
    char *p;

    if (*(int *)0x700031F4 >= 0xB) {
        return 0;
    }
    p = func_001AFA90(2);
    if (p != 0) {
        *(char *)(p + 3) = 0;
        *(char *)(p + 0xD) = (char)(arg2 + 0xB);
        *(char *)(p + 0x9A) = 0;
        *(float *)(p + 0xB0) = *(float *)(arg1 + 0);
        *(float *)(p + 0xB4) = *(float *)(arg1 + 4);
        *(float *)(p + 0xB8) = *(float *)(arg1 + 8);
        if (arg2 == 1) {
            *(float *)(p + 0xC0) = *(float *)(arg3 + 0);
            *(float *)(p + 0xC4) = *(float *)(arg3 + 4);
            *(float *)(p + 0xC8) = *(float *)(arg3 + 8);
        } else {
            *(int *)(p + 0xC0) = 0;
            *(int *)(p + 0xC4) = 0;
            *(int *)(p + 0xC8) = 0;
        }
        *(unsigned char *)(p + 0x9D) = *(unsigned char *)(arg0 + 0x9D);
        *(unsigned char *)(p + 0x9E) = *(unsigned char *)(arg0 + 0x9E);
        *(int *)(p + 0x60) = 0x3F800000;
        *(int *)(p + 0x64) = 0x3F800000;
        *(int *)(p + 0x68) = 0x3F800000;
        *(int *)(p + 0x6C) = 0x3F800000;
        *(int *)(p + 0x10) = (int)&func_0012A5D0;
        *(int *)0x700031F4 = *(int *)0x700031F4 + 1;
        return 1;
    }
    return 0;
}

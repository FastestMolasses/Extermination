// NEARMISS func_00113478  (vram 0x00113478, 0xB4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.78% via ee-gcc 2.9-991111-01 (-O2). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// eegcc %hi-CSE rematerialization wall (one instruction short: 44 vs 45). Improved 91.11% -> 97.78%. Structure, branch senses, register allocation and every other instruction match. Unlock found this pass: the failure-arm semaphore read must be `*(volatile int *)&D_00241D0C`, which exactly reproduc...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241D48;
extern int D_00241D0C;
extern int D_0027AB40;
extern int D_0027AF60;

extern int func_00112E28(int);
extern int func_0010E8A8(void *a, int b, int c, int d, int e, void *f, int g, int h, int i);
extern int CreateSema(int);

int func_00113478(void) {
    int *p;
    int r;
    if (func_00112E28(0x1E) == 0) {
        return 0;
    }
    p = &D_0027AB40;
    D_00241D48 = 8;
    r = func_0010E8A8(&D_0027AF60, 0x16, 0, 0, 0, p, 4, 0, 0);
    if (r < 0) {
        CreateSema(*(volatile int *)&D_00241D0C);
        D_00241D48 = 0;
        return 0;
    }
    D_00241D48 = 0;
    CreateSema(D_00241D0C);
    return *(int *)((unsigned int)p | 0x20000000);
}

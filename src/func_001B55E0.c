// NEARMISS func_001B55E0  (vram 0x001B55E0, 0x1A8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.40% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation/instruction-scheduling permutation. Body+structure fully recovered. Two residual regions: (1) mwcc recomputes arg0[+0xB0]/[+0xB8] addresses inline instead of the target's `paddub v1,a0` register copy + base+disp loads; (2) the FP epilogue scheduler interleaves cvt.s.w/div.s/0x...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern float func_001B1240(void *, float, float);
extern int func_001029C0(void *);
extern int func_00102BB0(void *, void *, float);
extern int func_001026A0(void *, void *, void *);
extern int func_001028B8(void *, void *, void *);
extern int func_00122BB8(void);
extern int func_001EFD90(int, void *, void *);
extern char D_00810360;
extern char D_70003400;
extern char D_70003600;
extern char D_70003610;

void func_001B55E0(char *arg0, int arg1) {
    int code;
    int sel;
    float temp;

    *(int *)0x70003618 = 0;
    *(float *)0x70003614 = 0.0f;
    *(int *)0x70003610 = 0;
    *(int *)0x7000361C = 0x3F800000;
    *(float *)0x70003614 = func_001B1240(&D_00810360, *(float *)(arg0 + 0xB0), *(float *)(arg0 + 0xB8));
    func_001029C0(&D_70003400);
    func_00102BB0(&D_70003400, &D_70003400, *(float *)0x70003614);
    *(int *)0x70003600 = 0;
    *(float *)0x70003604 = 0.0f;
    *(int *)0x70003608 = 0x40000000;
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(&D_70003600, &D_70003400, &D_70003600);
    func_001028B8(&D_70003600, &D_00810360, &D_70003600);
    sel = (arg1 << 0x18) >> 0x18;
    temp = 5.0f * ((float)((func_00122BB8() >> 5) & 0xFF) / 255.0f);
    *(float *)0x70003680 = temp;
    *(float *)0x70003604 = *(float *)0x70003604 + temp;
    switch (sel) {
    case 0:
        code = 0x80000006;
        break;
    case 1:
        code = 0x80000022;
        break;
    case 2:
        code = 0x80000023;
        break;
    }
    func_001EFD90(code, &D_70003600, &D_70003610);
}

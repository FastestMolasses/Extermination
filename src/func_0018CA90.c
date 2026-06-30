// NEARMISS func_0018CA90  (vram 0x0018CA90, 0x140 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.61% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP even/odd register-coloring + load-batch scheduling permutation. Body fully recovered. The three accumulator updates (D_008105D0 += D_008105E0; D_008105D4 += D_008105E4 + *(arg0+0x5C); D_008105D8 += D_008105E8) plus the '*(arg0+0x24) += 17.0f' update are computed by the target with all loads ba...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_00102948(void *, void *);
extern void func_001029C0(void *);
extern void func_00102C58(void *, void *, void *);
extern void func_001026A0(void *, void *, void *);
extern float D_008105D0;
extern float D_008105D4;
extern float D_008105D8;
extern float D_008105E0;
extern float D_008105E4;
extern float D_008105E8;
extern char D_70003400[];
extern char D_70003600[];
extern float D_70003B50;

void func_0018CA90(char *arg0, char *arg1) {
    func_00102948(arg0 + 0x30, &D_70003B50);
    func_00102948(arg0 + 0x20, arg1 + 0xA0);
    *(float *)(arg0 + 0x24) = *(float *)(arg0 + 0x24) + 17.0f;
    func_00102948(&D_008105E0, arg0 + 0x20);
    func_001029C0(D_70003400);
    func_00102C58(D_70003400, D_70003400, arg0 + 0x30);
    *(int *)0x70003600 = 0;
    *(int *)0x70003604 = 0;
    *(float *)0x70003608 = *(float *)(arg0 + 0xC);
    *(int *)0x7000360C = 0x3F800000;
    func_001026A0(&D_008105D0, D_70003400, D_70003600);
    D_008105D0 += D_008105E0;
    D_008105D4 += D_008105E4 + *(float *)(arg0 + 0x5C);
    D_008105D8 += D_008105E8;
    func_00102948(arg0 + 0x10, &D_008105D0);
}

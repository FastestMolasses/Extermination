// NEARMISS func_001A8660  (vram 0x001A8660, 0x1D4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.05% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// mwcc fuses the masked array-index multiply (d & 0xFF) * 4 into a single 'sll v0,a0,2' where the target emits a separate 'andi v0,a0,0xff' then 'sll v0,v0,2' (mask-then-shift peephole, not steerable from C without changing semantics); plus one delay-slot-fill difference at the 'bne a0,v1,...' earl...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 96.0% (mwcc233). Logic fully recovered; sole residuals are a
// mask+shift peephole (target emits andi+sll for (d&0xFF)*4, mwcc fuses to one
// sll) and one delay-slot fill on the early-out bne.
//
// Circle/box overlap test between two entities (arg0, arg1). Computes planar
// distance via func_0011E748 (length of dx,dy from arg0+0xA0/0xA8 vs
// arg1+0xB0/0xB8) and compares against the sum of radii at *(arg0+0x30)[0] +
// *(arg1+0x30)[0]; then an axis-overlap test on the 0xA4/0xB4 components using
// half-widths from [+4]. On overlap it fires the arg1 vtable handler at +0x34,
// and if arg0's state byte==1 picks a float from one of two tables
// (D_0024A740/D_0024A780, selected by global D_0081070A) indexed by arg1's
// type byte at +0xD, storing to arg0+0x22C (types 3,4) or arg0+0x224 (other),
// sets state=3, and initializes transform D_700038A0.
extern float func_0011E748(float);
extern int func_0021BD10(unsigned char);
extern int func_001028D0(void *, void *, void *);
extern int func_00102760(void *, void *);
extern float D_0024A740;
extern float D_0024A780;
extern unsigned char D_0081070A;
extern char D_700038A0;

void func_001A8660(char *arg0, char *arg1) {
    char *va;
    char *vb;
    float *base;
    float dx, dy;
    float t3, f2;
    int d;
    int idx;

    dx = *(float *)(arg0 + 0xA0) - *(float *)(arg1 + 0xB0);
    dy = *(float *)(arg0 + 0xA8) - *(float *)(arg1 + 0xB8);
    if (func_0011E748(dx * dx + dy * dy) <= (*(float *)*(char **)(arg0 + 0x30) + *(float *)*(char **)(arg1 + 0x30))) {
        va = *(char **)(arg0 + 0x30);
        vb = *(char **)(arg1 + 0x30);
        t3 = *(float *)(va + 4) / 2.0f;
        f2 = (*(float *)(arg0 + 0xA4) + t3) - *(float *)(arg1 + 0xB4);
        if (f2 < 0.0f) {
            f2 = -f2;
        }
        if (f2 <= (t3 + *(float *)(vb + 4) / 2.0f)) {
            (*(void (**)(char *, char *, char *))(arg1 + 0x34))(arg1, arg0, arg0 + 0xB0);
            if (*(unsigned char *)arg0 == 1) {
                if (*(unsigned char *)(arg1 + 0xD) == 0xB) {
                    if (func_0021BD10(*(unsigned char *)arg0) != 0) {
                        *(char *)(arg0 + 0xF) = 2;
                    }
                }
                if (D_0081070A == 0) {
                    base = &D_0024A740;
                } else {
                    base = &D_0024A780;
                }
                d = *(unsigned char *)(arg1 + 0xD);
                idx = d & 0xFF;
                if (d != 3) {
                    if (d == 4) {
                        goto store_22C;
                    }
                    *(float *)(arg0 + 0x224) = base[idx];
                } else {
store_22C:
                    *(float *)(arg0 + 0x22C) = base[idx];
                }
                *(unsigned char *)arg0 = 3;
                func_001028D0(&D_700038A0, arg0 + 0xA0, arg1 + 0xB0);
                *(int *)0x700038AC = 0x3F800000;
                func_00102760(arg0 + 0x70, &D_700038A0);
            }
            *(short *)0x70003B86 = 0;
        }
    }
}

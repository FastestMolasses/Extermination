// NEARMISS func_00150F40  (vram 0x00150F40, 0x2B4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.01% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-coloring permutation across the table-lookup block (see wall notes)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Per-limb IK/aim solver, run 4 times (arg1's 0x10-byte-strided sub-records). For each iteration:
// picks a row from D_00245F00 by arg1[0x65] (a small enum: which limb/side) and builds a target
// vector at 0x700038A0..AC; copy_qw4 pulls a bone matrix (D_00275B40[row-index]+0x90) into
// D_700036A0, func_001026A0 transforms the target into that bone space; a heading angle is derived
// via func_001B1240 against D_00810360/368 and clamped to +-30 degrees (0.5235988f) of the current
// aim (func_001B1470 wraps); func_0014D1E0 + func_001EFD90(3,...) apply/dispatch the aim result.
// After all 4 iterations, func_001FBD50(arg0, 0x874, 0, 500.0f) fires a completion event.

extern void copy_qw4(char *dst, char *src);
extern int func_001026A0(void *, void *, void *);
extern void func_0014D1E0(void *a, void *b, int n);
extern float func_001B1240(void *origin, float x, float z);
extern float func_001B1470(float a);
extern void func_001EFD90(int id, void *a, void *b);
extern void func_001FBD50(unsigned char *p, int a, int b, float f);

extern char D_00245F00[];
extern int D_00275B40;
extern float D_00810360[8];
extern float D_00810368[8];
extern int D_700036A0[8];
extern int D_700038A0[8];
extern int D_700038B0[8];

void func_00150F40(unsigned char *arg0, unsigned char *arg1) {
    int i;
    int off;

    i = 0;
    off = 0;
    do {
        char *row;
        signed char idx;
        int *tbl;

        tbl = &D_00275B40;
        idx = (signed char)arg1[0x65];
        row = D_00245F00 + (idx << 6) + off;
        *(float *)0x700038A0 = *(float *)(row + 4);
        idx = (signed char)arg1[0x65];
        row = D_00245F00 + (idx << 6) + off;
        *(float *)0x700038A4 = *(float *)(row + 8);
        idx = (signed char)arg1[0x65];
        row = D_00245F00 + (idx << 6) + off;
        *(float *)0x700038A8 = *(float *)(row + 0xC);
        *(int *)0x700038AC = 0x3F800000;
        idx = (signed char)arg1[0x65];
        row = D_00245F00 + (idx << 6) + off;
        copy_qw4((char *)D_700036A0, (char *)*(int *)(tbl + *(int *)row) + 0x90);
        func_001026A0(D_700038A0, D_700036A0, D_700038A0);
        *(int *)0x700038B8 = 0;
        *(int *)0x700038B4 = 0;
        *(int *)0x700038B0 = 0;
        *(int *)0x700038BC = 0x3F800000;
        idx = (signed char)arg1[0x65];
        if (idx != 2) {
            if (idx != 0) {
                *(float *)0x700038B4 = -1.5707964f;
            } else {
                *(float *)0x700038B4 = 1.5707964f;
            }
        }
        *(float *)0x70003A20 = func_001B1240(D_700038A0, D_00810360[0], D_00810368[0]);
        if (func_001B1470(0.5235988f + *(float *)0x700038B4) < *(float *)0x70003A20) {
            *(float *)0x700038B4 = func_001B1470(0.5235988f + *(float *)0x700038B4);
        } else if (!(func_001B1470(*(float *)0x700038B4 - 0.5235988f) <= *(float *)0x70003A20)) {
            *(float *)0x700038B4 = func_001B1470(*(float *)0x700038B4 - 0.5235988f);
        } else {
            *(float *)0x700038B4 = *(float *)0x70003A20;
        }
        func_0014D1E0(D_700038A0, D_700038B0, 0);
        func_001EFD90(3, D_700038A0, D_700038B0);
        i++;
        off += 0x10;
    } while (i < 4);
    func_001FBD50(arg0, 0x874, 0, 500.0f);
}

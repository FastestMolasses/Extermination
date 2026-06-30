// NEARMISS func_00151200  (vram 0x00151200, 0x1A8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.98% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring + branch-likely-sense permutation in the 4-iteration loop. Body + structure fully recovered and byte-identical through the scratch-block / call sequence. Three residual artifacts, all compiler-internal: (1) s0<->s1 swap -- target colors s0=loop counter / s1=D_00245FC0 cursor, mw...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS 90.98% (mwcc 2.3.3; 991202 = 80.6%). Body + structure fully recovered.
// Residuals are compiler-internal: (1) s0<->s1 swap (target s0=counter/s1=cursor,
// mwcc opposite); (2) entry ptr e=arg1+i kept callee-saved by mwcc vs recomputed
// into a temp by CodeWarrior; (3) branch-likely sense (mwcc beqzl vs target beqz)
// on the two loop guards. -sdatathreshold>=4 keeps D_00275B40 gp-relative while the
// other globals stay absolute. Regalloc/branch-likely permutation -> permuter.
//
// If func_001B1EA0(0, self+0xB0, D_00246000, 4) passes, loops over 4 entries at
// arg1+i: for each with byte +0x6C < 2, decrement the +0x68 cooldown if nonzero,
// else bump +0x6C and reseed +0x68 = (func_00122BB8(e, e+0x6C)>>14)&0x1F, then seed
// the scratch vec4 at 0x700038A0 from the D_00245FC0[i] row (floats +4/+8/+0xC,
// w=1.0f), copy the bone matrix *(D_00275B40 + row[0]*4)+0x90 into 0x700036A0,
// combine via func_001026A0, zero the 0x700038B0 block (w=1.0f), then
// func_001EFD90(5,..), func_001283D0(scratch,0,self+0xC0), and
// func_001FBD50(self, 0x876, 0, 500.0f).
extern int func_001B1EA0(int, int, void *, int);
extern int func_00122BB8(char *, char *);
extern void copy_qw4(void *, void *);
extern void func_001026A0(void *, void *, void *);
extern void func_001EFD90(int, void *, void *);
extern void func_001283D0(void *, int, int);
extern void func_001FBD50(int, int, int, float);
extern int D_00275B40;
extern int D_00245FC0[];
extern unsigned char D_00246000[0x40];
extern char D_700036A0[];
extern char D_700038A0[];
extern char D_700038B0[];

void func_00151200(int arg0, int arg1) {
    int *p;
    int i;
    char *e;

    if (func_001B1EA0(0, arg0 + 0xB0, D_00246000, 4) != 0) {
        i = 0;
        p = D_00245FC0;
        do {
            e = (char *)(arg1 + i);
            if (*(char *)(e + 0x6C) < 2) {
                if (*(char *)(e + 0x68) != 0) {
                    *(char *)(e + 0x68) = *(char *)(e + 0x68) - 1;
                } else {
                    *(char *)(e + 0x6C) = *(char *)(e + 0x6C) + 1;
                    *(char *)(e + 0x68) = (func_00122BB8(e, e + 0x6C) >> 0xE) & 0x1F;
                    *(float *)0x700038A0 = *(float *)((char *)p + 4);
                    *(float *)0x700038A4 = *(float *)((char *)p + 8);
                    *(float *)0x700038A8 = *(float *)((char *)p + 0xC);
                    *(int *)0x700038AC = 0x3F800000;
                    copy_qw4(D_700036A0, (char *)*(int *)(D_00275B40 + p[0] * 4) + 0x90);
                    func_001026A0(D_700038A0, D_700036A0, D_700038A0);
                    *(int *)0x700038B8 = 0;
                    *(int *)0x700038B4 = 0;
                    *(int *)0x700038B0 = 0;
                    *(int *)0x700038BC = 0x3F800000;
                    func_001EFD90(5, D_700038A0, D_700038B0);
                    func_001283D0(D_700038A0, 0, arg0 + 0xC0);
                    func_001FBD50(arg0, 0x876, 0, 500.0f);
                }
            }
            i += 1;
            p += 4;
        } while (i < 4);
    }
}

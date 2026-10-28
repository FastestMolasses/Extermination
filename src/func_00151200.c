// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// SPAD: 0x700038A4 0x700038A8 0x700038AC
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 93.43% NEARMISS).
// Fixes read from the original instructions:
//  - the per-slot bytes are arrays indexed by i (cooldown at +0x68, count at +0x6C
//    of arg1), not a byte pointer arg1+i; this gave the original's address CSE and
//    the counter/cursor register assignment;
//  - self (arg0) is a pointer, so the func_001283D0 arguments set up in order;
//  - the scratch vec4 words at 0x700038A4/A8/AC are relocated scratchpad externs
//    (idiom-32), which lets the D_00275B40 load move above the first store;
//    w is stored as 1.0f.
//
// If func_001B1EA0(0, self+0xB0, D_00246000, 4) passes, loops over 4 slots of
// arg1: for each with count byte (+0x6C+i) < 2, decrement the cooldown byte
// (+0x68+i) if nonzero, else bump the count and reseed the cooldown with
// (func_00122BB8()>>14)&0x1F, then seed the scratch vec4 at 0x700038A0 from the
// D_00245FC0[i] row (floats +4/+8/+0xC, w=1.0f), copy the bone matrix
// *(D_00275B40 + row[0]*4)+0x90 into 0x700036A0, combine via func_001026A0,
// zero the 0x700038B0 block (w=1.0f), then func_001EFD90(5,..),
// func_001283D0(scratch,0,self+0xC0), and func_001FBD50(self, 0x876, 0, 500.0f).
extern int func_001B1EA0(int, void *, void *, int);
extern int func_00122BB8(void);
extern void copy_qw4(void *, void *);
extern void func_001026A0(void *, void *, void *);
extern int func_001EFD90(int, void *, void *);
extern int func_001283D0(void *, int, void *);
extern int func_001FBD50(void *, int, int, float);
extern int D_00275B40;
extern int D_00245FC0[];
extern unsigned char D_00246000[0x40];
extern char D_700036A0[];
extern float D_700038A0[];
extern float D_700038A4[];
extern float D_700038A8[];
extern float D_700038AC[];
extern char D_700038B0[];

/* Four per-slot bytes at +0x68 (cooldown) and +0x6C (count). */
typedef struct {
    char pad[0x68];
    signed char cd[4];
    signed char cnt[4];
} Ent;

void func_00151200(char *arg0, Ent *arg1) {
    int i;
    int *p;

    if (func_001B1EA0(0, arg0 + 0xB0, D_00246000, 4) != 0) {
        for (i = 0, p = D_00245FC0; i < 4; i++, p += 4) {
            if (arg1->cnt[i] < 2) {
                if (arg1->cd[i] != 0) {
                    arg1->cd[i]--;
                } else {
                    arg1->cnt[i]++;
                    arg1->cd[i] = (func_00122BB8() >> 0xE) & 0x1F;
                    D_700038A0[0] = *(float *)((char *)p + 4);
                    D_700038A4[0] = *(float *)((char *)p + 8);
                    D_700038A8[0] = *(float *)((char *)p + 0xC);
                    D_700038AC[0] = 1.0f;
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
        }
    }
}

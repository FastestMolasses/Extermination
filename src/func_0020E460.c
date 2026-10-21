// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// TU_STATIC: func_0020E3A0

//
// Byte-matched (objdiff 100%), decomp2 lane round 4 (was a 93.18% NEARMISS).
// Fixes read from the original instructions:
//  - The fallback state calls func_001AFF90(self): the target sets only $a0
//    and func_001AFF90 takes one argument (the old C passed (self, 3, state)).
//  - func_0020E3A0 takes one argument (the glyph code). It is compiled as a
//    static function of this file (// TU_STATIC:, MATCHING_GUIDE "Decomp lane
//    round 7"): the original compiler knew it leaves $a1 alone, so state 1
//    loads self[0xD] into $a1 BEFORE the call and compares the result with it
//    afterwards. That was the old header's "saved id in $a1" residual. The
//    callee's switch has its own jump table; tu_static.py now checks that
//    table against the original and removes it with the copy.
//  - -sdatathreshold 4: D_00275BCC and D_00275B40 are gp-relative in the
//    target; the 0x008106xx presets and D_0024A340 are declared as arrays so
//    they stay absolute.
//
// Actor state-machine tick, dispatched on state byte at arg0+4 (0..3).
// state 0: if bone count arg0+0xC exceeds the cap D_00275BCC, jump straight
//   to state 3 and bail. Otherwise allocate a func_001AF7C0() slot into the
//   arg0+0x110[] pointer array for each bone, mirror the count to arg0+9,
//   run anim_bone_array_setup(count) + bone_init_default_1(self) +
//   func_001CA5F0(self, 0xB), seed scale floats +0x80/+0x84/+0x88 = 1.5,
//   zero +0xC0/+0xC8, set heading +0xC4 = pi, advance state to 1, then
//   accumulate a weighted position at +0xB0/+0xB4/+0xB8 from the disc
//   preset table D_00810610..3C (base + 40*mid - 18.4*lo + 1.3*hi per axis).
// state 1: func_001C6380(self) (world-matrix build); if
//   func_0020E3A0(0x15) == self[0xD] (self[0xD] read before the call) fetch
//   the row ptr at *D_00275B40 and blend it via func_001026A0(row+0xC0,
//   row+0x90, D_0024A340). Freezes input (func_001D2040(0,1)), calls the
//   vtable tick fn at self+0x4C(self), then unfreezes (func_001D2040(0,0)).
// state 2 / state 3 / default: func_001AFF90(self) (fallback).
extern void func_001C6380(unsigned char *self);
extern int func_001AF7C0(void);
extern void anim_bone_array_setup(int n);
extern void bone_init_default_1(unsigned char *self);
extern void func_001CA5F0(unsigned char *self, int mode);
static int func_0020E3A0(int code);  /* same-TU callee, see TU_STATIC */
#include "src/func_0020E3A0.c"
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001D2040(int a0, int a1);
extern void func_001AFF90(unsigned char *self);
extern short D_00275BCC;
extern int D_00275B40;
extern int D_0024A340[];
extern float D_00810610[];
extern float D_00810614[];
extern float D_00810618[];
extern float D_0081061C[];
extern float D_00810620[];
extern float D_00810624[];
extern float D_00810628[];
extern float D_0081062C[];
extern float D_00810630[];
extern float D_00810634[];
extern float D_00810638[];
extern float D_0081063C[];

void func_0020E460(unsigned char *arg0) {
    int i;
    unsigned char *q;
    unsigned char n;
    int row;
    unsigned char ch;

    switch (arg0[4]) {
    case 0:
        i = 0;
        if (D_00275BCC < (int)arg0[0xC]) {
            arg0[4] = 3;
            return;
        }
        for (i = 0, q = arg0; i < (int)arg0[0xC]; i++) {
            *(int *)(q + 0x110) = func_001AF7C0();
            q += 4;
        }
        n = arg0[0xC];
        arg0[9] = n;
        anim_bone_array_setup(arg0[0xC]);
        bone_init_default_1(arg0);
        func_001CA5F0(arg0, 0xB);
        *(int *)(arg0 + 0x80) = 0x3FC00000;
        *(int *)(arg0 + 0x84) = 0x3FC00000;
        *(int *)(arg0 + 0x88) = 0x3FC00000;
        *(int *)(arg0 + 0xC0) = 0;
        *(int *)(arg0 + 0xC4) = 0x40490FDB;
        *(int *)(arg0 + 0xC8) = 0;
        arg0[4] = 1;
        *(float *)(arg0 + 0xB0) = D_0081061C[0];
        *(float *)(arg0 + 0xB4) = D_0081062C[0];
        *(float *)(arg0 + 0xB8) = D_0081063C[0];
        *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + 40.0f * D_00810618[0];
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + 40.0f * D_00810628[0];
        *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + 40.0f * D_00810638[0];
        *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + -18.4f * D_00810610[0];
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + -18.4f * D_00810620[0];
        *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + -18.4f * D_00810630[0];
        *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + 1.3f * D_00810614[0];
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + 1.3f * D_00810624[0];
        *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + 1.3f * D_00810634[0];
        return;
    case 1:
        func_001C6380(arg0);
        ch = arg0[0xD];
        if (func_0020E3A0(0x15) == ch) {
            row = *(int *)D_00275B40;
            func_001026A0((void *)(row + 0xC0), (void *)(row + 0x90), D_0024A340);
        }
        func_001D2040(0, 1);
        (*(void (**)(unsigned char *))(arg0 + 0x4C))(arg0);
        func_001D2040(0, 0);
        return;
    case 2:
    case 3:
    default:
        func_001AFF90(arg0);
        return;
    }
}

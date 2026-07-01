// NEARMISS func_0020E460  (vram 0x0020E460, 0x28C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.18% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Switch-dispatch register-coloring / delay-slot-hoist / CSE-sharing scheduling artifact: target shares the state-1 exit and the case-2/3/default tail call through one merged epilogue and hoists the func_001AFF90 self-arg move into the case-2 branch's delay slot, keeping the state-1 saved id in $a1...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

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
//   func_0020E3A0(0x15, self[0xD]) == self[0xD] fetch the row ptr at
//   *D_00275B40 and blend it via func_001026A0(row+0xC0, row+0x90,
//   &D_0024A340). Freezes input (func_001D2040(0,1)), calls the vtable tick
//   fn at self+0x4C(self), then unfreezes (func_001D2040(0,0)).
// state 2 / state 3 / default: func_001AFF90(self, 3, state) (fallback).
//
// NEARMISS (mwcc233 93.18%, mwcc 991202 91.04%). Logic and control-flow
// fully recovered -- every store/load/call/branch condition matches. The
// residual is a switch-dispatch register-coloring/scheduling artifact:
// target hoists the func_001AFF90 self-arg move ($a0=$s0) into the delay
// slot of the case-2 test and keeps state-1's saved id in $a1 across the
// func_0020E3A0 call + shares the state-1 exit with the case-2/3/default
// tail call through one merged epilogue block; mwcc instead re-derives the
// self pointer at the call site and allocates a fresh $s1 for the saved id,
// giving each arm its own (slightly larger) tail. Tried: ascending-case
// switch (this file, best result), if/else descending cascade (worse,
// 60%), case-2/3 hoisted first (worse). Parked as a scheduling/CSE-sharing
// wall -- no source reshape found that makes mwcc merge the tails the way
// the target does.
extern void func_001C6380(unsigned char *self);
extern int func_001AF7C0(void);
extern void anim_bone_array_setup(int n);
extern void bone_init_default_1(unsigned char *self);
extern void func_001CA5F0(unsigned char *self, int mode);
extern int func_0020E3A0(int a0, unsigned char a1);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001D2040(int a0, int a1);
extern void func_001AFF90(unsigned char *self, int a1, unsigned char a2);
extern short D_00275BCC;
extern int D_00275B40;
extern int D_0024A340;
extern float D_00810610;
extern float D_00810614;
extern float D_00810618;
extern float D_0081061C;
extern float D_00810620;
extern float D_00810624;
extern float D_00810628;
extern float D_0081062C;
extern float D_00810630;
extern float D_00810634;
extern float D_00810638;
extern float D_0081063C;

void func_0020E460(unsigned char *arg0) {
    int i;
    unsigned char *q;
    unsigned char n;
    int row;

    switch (arg0[4]) {
    case 0:
        i = 0;
        if (D_00275BCC < (int)arg0[0xC]) {
            arg0[4] = 3;
            return;
        }
        q = arg0;
        while (i < (int)arg0[0xC]) {
            *(int *)(q + 0x110) = func_001AF7C0();
            q += 4;
            i += 1;
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
        *(float *)(arg0 + 0xB0) = D_0081061C;
        *(float *)(arg0 + 0xB4) = D_0081062C;
        *(float *)(arg0 + 0xB8) = D_0081063C;
        *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + 40.0f * D_00810618;
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + 40.0f * D_00810628;
        *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + 40.0f * D_00810638;
        *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + -18.4f * D_00810610;
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + -18.4f * D_00810620;
        *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + -18.4f * D_00810630;
        *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + 1.3f * D_00810614;
        *(float *)(arg0 + 0xB4) = *(float *)(arg0 + 0xB4) + 1.3f * D_00810624;
        *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + 1.3f * D_00810634;
        return;
    case 1:
        func_001C6380(arg0);
        if (func_0020E3A0(0x15, arg0[0xD]) == arg0[0xD]) {
            row = *(int *)D_00275B40;
            func_001026A0((void *)(row + 0xC0), (void *)(row + 0x90), &D_0024A340);
        }
        func_001D2040(0, 1);
        (*(void (**)(unsigned char *))(arg0 + 0x4C))(arg0);
        func_001D2040(0, 0);
        return;
    case 2:
    case 3:
    default:
        func_001AFF90(arg0, 3, arg0[4]);
        return;
    }
}

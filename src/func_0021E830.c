// NEARMISS func_0021E830  (vram 0x0021E830, 0x184 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.38% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// branch-likely-sense permutation: the case-1 float guard `if(arg0[0x3C] <= 160.0f)` emits a CW `bc1fl` (branch-likely, delay slot speculates `a0 = arg0` for the always-following func_0021E650 call) where mwcc 2.3.3 emits plain `bc1f` with the same delay-slot op. Single-instruction likely-bit diff;...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Per-state actor driver dispatched on the state byte at arg0+6 (4-way switch
// 0/1/2/3), with a common tail func_00179880(arg0, arg0+0x2EC) +
// func_00175900(arg0, 1) run for every state.
//   state 0: advance state, clear arg0+7, kick clip 0x1C4 via
//            func_001749A0, register func_001EFE00(0x80000051, self), trigger
//            func_001B61C0(0,0xC0,5,1), fire two func_001FBD50 sound/anim cues
//            (0x146 and 0x151 at 300.0f), clear the timer at arg0+0x2EC.
//   state 1: once the float at arg0+0x3C has dropped to <= 160.0, advance
//            state, set arg0+0x234 = 2 and the global D_00810707 = 2, and call
//            func_0015C1F0(); always run func_0021E650(self) afterward.
//   state 2: if the 0x1000 flag is set in arg0+0x200, advance state, clear
//            arg0+7 and trigger func_001B61C0(1,0xEE,0x3C,1); else just run
//            func_0021E650(self).
//   state 3: func_0021D2E0(self, 0x78, 1).
//
// WALL: branch-likely-sense permutation. The state-1 guard
// `if (arg0[0x3C] <= 160.0f)` lowers to a CW `bc1fl` (branch-likely) whose
// delay slot speculates the `a0 = arg0` setup for the always-following
// func_0021E650 call; mwcc 2.3.3 emits plain `bc1f` with the same delay-slot
// op. Identical operands, only the likely bit differs (one instruction). Since
// func_0021E650 is the post-if merge reached on both paths, no source reshape
// induces mwcc's likely form - not the clean-store nop; parked.
extern void func_001749A0(unsigned char *e, short clip, int flags, float blend);
extern void func_001EFE00(int a, unsigned char *e);
extern void func_001B61C0(int a, int b, int c, int d);
extern void func_001FBD50(unsigned char *e, int a, int b, float f);
extern void func_0015C1F0(void);
extern void func_0021E650(unsigned char *e);
extern void func_0021D2E0(unsigned char *e, short a, int b);
extern void func_00179880(unsigned char *e, void *p);
extern void func_00175900(unsigned char *e, int f);
extern unsigned char D_00810707;

void func_0021E830(unsigned char *arg0) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(unsigned char *)(arg0 + 7) = 0;
        func_001749A0(arg0, 0x1C4, 0, 0.0f);
        func_001EFE00(0x80000051, arg0);
        func_001B61C0(0, 0xC0, 5, 1);
        func_001FBD50(arg0, 0x146, 0, 300.0f);
        func_001FBD50(arg0, 0x151, 0, 300.0f);
        *(int *)(arg0 + 0x2EC) = 0;
        break;
    case 1:
        if (*(float *)(arg0 + 0x3C) <= 160.0f) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(unsigned char *)(arg0 + 0x234) = 2;
            D_00810707 = 2;
            func_0015C1F0();
        }
        func_0021E650(arg0);
        break;
    case 2:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(unsigned char *)(arg0 + 7) = 0;
            func_001B61C0(1, 0xEE, 0x3C, 1);
        } else {
            func_0021E650(arg0);
        }
        break;
    case 3:
        func_0021D2E0(arg0, 0x78, 1);
        break;
    }
    func_00179880(arg0, arg0 + 0x2EC);
    func_00175900(arg0, 1);
}

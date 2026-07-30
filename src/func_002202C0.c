// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// SEMANTICS: cut-scene / set-piece actor state machine for entity e, dispatched
// on the state byte e[6] through jtbl_002738A0 (14 entries; state 9 is empty and
// any index >= 14 falls straight to the shared tail).
//   0  - func_001FBD50(e, 0x150, 0, 300.0f), advance, clear e[7], play anim 0x32
//        at 1.0 and reset the root-motion accumulators e+0x38 / e+0x21C.
//   1  - on input flag 0x1000: advance, cache the clip length
//        func_001C61D0(e+0x40, 0x26) at 0x70003A20 and hand it to
//        anim_clip_arbiter(e, 0x26, 0.0f, len - 30.0f), then re-base e+0x21C on the
//        global clock (*D_00275B40)[8] and clear e+0x2E4.  Otherwise integrate:
//        e+0x38 = clock8 - e[0x21C], e+0x2EC = clock4 - e[0x2E4],
//        e+0xB4 += e+0x2EC, with func_00178B90(e, 1) / func_00175900(e, 1).
//   2  - same integration as state 1 but scaled by 0.75; advances on flag 0x1000.
//   3  - func_00178B90(e,1) + publish the 0x2EC block; when func_00175900 reports
//        done, advance, func_001FBD50(e, 0x12F, 0, 300.0f), func_001B61C0(0,0xD0,0xA,1),
//        play anim 0x27 and reset the accumulators.
//   4  - on flag 0x1000: if e+0x228 >= 100.0f and the global D_008106F1 is set,
//        advance one state, func_0021C120(e) and set e[0x302]=1; otherwise skip a
//        state (+2) and clear e[0x302].  Off-flag it just integrates the clock.
//   5  - advance when func_0021C190(e) reports done.
//   6  - if e+0x220 != 0 advance and play anim 0x28 at 12.0 (resetting the
//        accumulators), else jump to state 0xA and play anim 0x29 at 16.0.
//   7  - advance and clear e[0x1F1] once flag 0x8000 clears.
//   8  - on flag 0x1000 reset the hit bookkeeping (e[0xF], e+0x224, e+0x22C,
//        e+0x20E = 60 ticks, e[0x25C]) and func_0017C540(e); otherwise integrate the
//        clock and, when e[0x302] is set, force the blend rate e+0x204 to 0.4f.
//   10 - advance once flag 0x8000 clears.
//   11 - advance and func_0021D490(e) while e+0x3C <= 24.0f.
//   12 - on flag 0x1000 advance, clear e[7] and func_001B61C0(1, 0xEE, 0x3C, 1).
//   13 - func_0021D2E0(e, 0x78, 0).
// States 3..13 (except 13) all end by publishing the 0x2EC root-motion block with
// func_00179880(e, e+0x2EC) / func_00175900(e, 1).  The shared tail fires
// func_0021D250(e, 0) when the actor id byte e[0x23A] is 0x5D.
extern void anim_clip_arbiter(char *e, int clip, float speed, float f);
extern int func_001749A0(char *e, int id, int flag, float v);
extern int func_00175900(char *e, int a);
extern int func_00178B90(char *e, int a);
extern int func_00179880(char *e, char *p);
extern int func_0017C540(char *e);
extern int func_001B61C0(int a, int b, int c, int d);
extern int func_001C61D0(int a, int b);
extern void func_001FBD50(char *e, int id, int flag, float v);
extern int func_0021C120(char *e);
extern int func_0021C190(char *e);
extern int func_0021D250(char *e, int a);
extern int func_0021D2E0(char *e, int a, int b);
extern int func_0021D490(char *e);
extern char *D_00275B40;
extern unsigned char D_008106F1[16];

void func_002202C0(char *e) {
    unsigned char st;
    float zero;

    st = *(unsigned char *)(e + 6);
    switch (st) {
    case 0:
        func_001FBD50(e, 0x150, 0, 300.0f);
        *(unsigned char *)(e + 6) = *(unsigned char *)(e + 6) + 1;
        *(char *)(e + 7) = 0;
        func_001749A0(e, 0x32, 0, 1.0f);
        *(float *)(e + 0x38) = 0.0f;
        *(float *)(e + 0x21C) = 0.0f;
        break;
    case 1:
        if (*(int *)(e + 0x200) & 0x1000) {
            *(unsigned char *)(e + 6) = st + 1;
            *(float *)0x70003A20 = (float)func_001C61D0(*(int *)(e + 0x40), 0x26);
            anim_clip_arbiter(e, 0x26, 0.0f, *(float *)0x70003A20 - 30.0f);
            *(float *)(e + 0x38) = 0.0f;
            *(float *)(e + 0x21C) = *(float *)(*(char **)D_00275B40 + 8);
            *(float *)(e + 0x2E4) = 0.0f;
        } else {
            *(float *)(e + 0x38) = *(float *)(*(char **)D_00275B40 + 8) - *(float *)(e + 0x21C);
            *(float *)(e + 0x21C) = *(float *)(*(char **)D_00275B40 + 8);
            func_00178B90(e, 1);
            *(float *)(e + 0x2EC) = *(float *)(*(char **)D_00275B40 + 4) - *(float *)(e + 0x2E4);
            *(float *)(e + 0x2E4) = *(float *)(*(char **)D_00275B40 + 4);
            *(float *)(e + 0xB4) += *(float *)(e + 0x2EC);
            func_00175900(e, 1);
        }
        break;
    case 2:
        if (*(int *)(e + 0x200) & 0x1000) {
            *(unsigned char *)(e + 6) = st + 1;
        } else {
            *(float *)(e + 0x38) = *(float *)(*(char **)D_00275B40 + 8) - *(float *)(e + 0x21C);
            *(float *)(e + 0x21C) = *(float *)(*(char **)D_00275B40 + 8);
            *(float *)(e + 0x38) *= 0.75f;
            func_00178B90(e, 1);
            *(float *)(e + 0x2EC) = *(float *)(*(char **)D_00275B40 + 4) - *(float *)(e + 0x2E4);
            *(float *)(e + 0x2E4) = *(float *)(*(char **)D_00275B40 + 4);
            *(float *)(e + 0xB4) += *(float *)(e + 0x2EC);
            func_00175900(e, 1);
        }
        break;
    case 3:
        func_00178B90(e, 1);
        func_00179880(e, e + 0x2EC);
        if (func_00175900(e, 1) != 0) {
            *(unsigned char *)(e + 6) = *(unsigned char *)(e + 6) + 1;
            func_001FBD50(e, 0x12F, 0, 300.0f);
            func_001B61C0(0, 0xD0, 0xA, 1);
            func_001749A0(e, 0x27, 0, 0.0f);
            *(float *)(e + 0x38) = 0.0f;
            *(float *)(e + 0x21C) = 0.0f;
        }
        break;
    case 4:
        if (*(int *)(e + 0x200) & 0x1000) {
            if (!(*(float *)(e + 0x228) < 100.0f) && D_008106F1[0] != 0) {
                *(unsigned char *)(e + 6) = st + 1;
                func_0021C120(e);
                *(char *)(e + 0x302) = 1;
            } else {
                *(unsigned char *)(e + 6) = *(volatile unsigned char *)(e + 6) + 2;
                *(char *)(e + 0x302) = 0;
            }
        } else {
            *(float *)(e + 0x38) = *(float *)(*(char **)D_00275B40 + 8) - *(float *)(e + 0x21C);
            *(float *)(e + 0x21C) = *(float *)(*(char **)D_00275B40 + 8);
            func_00178B90(e, 1);
        }
        func_00179880(e, e + 0x2EC);
        func_00175900(e, 1);
        break;
    case 5:
        if (func_0021C190(e) != 0) {
            *(unsigned char *)(e + 6) = *(unsigned char *)(e + 6) + 1;
        }
        func_00179880(e, e + 0x2EC);
        func_00175900(e, 1);
        break;
    case 6:
        zero = 0.0f;
        if (*(float *)(e + 0x220) != zero) {
            *(unsigned char *)(e + 6) = st + 1;
            func_001749A0(e, 0x28, 0, 12.0f);
            *(float *)(e + 0x38) = 0.0f;
            *(float *)(e + 0x21C) = 0.0f;
        } else {
            *(unsigned char *)(e + 6) = 0xA;
            func_001749A0(e, 0x29, 0, 16.0f);
        }
        func_00179880(e, e + 0x2EC);
        func_00175900(e, 1);
        break;
    case 7:
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            *(unsigned char *)(e + 6) = st + 1;
            *(char *)(e + 0x1F1) = 0;
        }
        func_00179880(e, e + 0x2EC);
        func_00175900(e, 1);
        break;
    case 8:
        if (*(int *)(e + 0x200) & 0x1000) {
            *(char *)(e + 0xF) = 0;
            *(int *)(e + 0x224) = 0;
            *(int *)(e + 0x22C) = 0;
            *(short *)(e + 0x20E) = 0x3C;
            *(char *)(e + 0x25C) = 0;
            func_0017C540(e);
        } else {
            *(float *)(e + 0x38) = *(float *)(*(char **)D_00275B40 + 8) - *(float *)(e + 0x21C);
            *(float *)(e + 0x21C) = *(float *)(*(char **)D_00275B40 + 8);
            func_00178B90(e, 1);
            if (*(unsigned char *)(e + 0x302) != 0) {
                *(float *)(e + 0x204) = 0.4f;
            }
        }
        func_00179880(e, e + 0x2EC);
        func_00175900(e, 1);
        break;
    case 9:
        break;
    case 10:
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            *(unsigned char *)(e + 6) = st + 1;
        }
        func_00179880(e, e + 0x2EC);
        func_00175900(e, 1);
        break;
    case 11:
        if (*(float *)(e + 0x3C) <= 24.0f) {
            *(unsigned char *)(e + 6) = st + 1;
            func_0021D490(e);
        }
        func_00179880(e, e + 0x2EC);
        func_00175900(e, 1);
        break;
    case 12:
        if (*(int *)(e + 0x200) & 0x1000) {
            *(unsigned char *)(e + 6) = st + 1;
            *(char *)(e + 7) = 0;
            func_001B61C0(1, 0xEE, 0x3C, 1);
        }
        func_00179880(e, e + 0x2EC);
        func_00175900(e, 1);
        break;
    case 13:
        func_0021D2E0(e, 0x78, 0);
        break;
    }
    if (*(unsigned char *)(e + 0x23A) == 0x5D) {
        func_0021D250(e, 0);
    }
}

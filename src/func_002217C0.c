// NEARMISS func_002217C0  (vram 0x002217C0, 0x4B0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.75% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-permutation wall in the switch dispatch chain: body/structure/frame are byte-identical (single saved reg s0=actor pointer, matches target exactly after fixing the torso-local and D_00275B40-vs-reload bugs); the sole residual is that mwcc233 keeps the dispatch value in v1 (rotating a1/a2/...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Actor top-level state machine dispatch on state byte +6 (values 0..0xD,
// plus terminal states 0x63). Each state advances +6, plays clips/sounds,
// and falls through to the shared "hit-stop" check on +0x23A==0x5D at the
// end (func_0021D250).
extern void func_001749A0(unsigned char *e, int clip, int flags, float blend);
extern int func_00175900(unsigned char *e, int a1);
extern void func_001764E0(unsigned char *e, unsigned char *torso);
extern void func_00178B90(unsigned char *e, int a1);
extern void func_00179880(unsigned char *e, unsigned char *torso);
extern void func_0017C540(unsigned char *e);
extern void func_001B61C0(int a0, int a1, int a2, int a3);
extern void func_001FBD50(unsigned char *e, int id, int a2, float radius);
extern void func_0021D250(unsigned char *e, int a1);
extern void func_0021D2E0(int a0, int a1);
extern void func_0021D490(unsigned char *e);
extern unsigned char **D_00275B40;

void func_002217C0(unsigned char *e) {
    unsigned char st;

    st = e[6];
    switch (st) {
    case 0:
        func_001B61C0(0, 0xC0, 5, 1);
        func_001FBD50(e, 0x150, 0, 300.0f);
        e[6] = e[6] + 1;
        func_001749A0(e, 0x3B, 0, 1.0f);
        *(int *)(e + 0x38) = 0;
        *(int *)(e + 0x21C) = 0;
        break;
    case 1:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[6] = st + 1;
        } else {
            *(float *)(e + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(e + 0x21C);
            *(float *)(e + 0x21C) = *(float *)(*D_00275B40 + 8);
            *(float *)(e + 0x2EC) = *(float *)(*D_00275B40 + 4) - *(float *)(e + 0x2E4);
            *(float *)(e + 0x2E4) = *(float *)(*D_00275B40 + 4);
            *(float *)(e + 0xB4) = *(float *)(e + 0xB4) + *(float *)(e + 0x2EC);
            func_00178B90(e, 1);
            if (*(unsigned char *)(e + 0x314) & 0xE0) {
                e[6] = 3;
                func_001749A0(e, 0x1C5, 0, 1.0f);
            }
            func_00175900(e, 1);
        }
        break;
    case 2:
        func_00179880(e, e + 0x2EC);
        func_00178B90(e, 1);
        if (*(unsigned char *)(e + 0x314) & 0xE0) {
            e[6] = 3;
            func_001749A0(e, 0x1C5, 0, 1.0f);
        }
        if (func_00175900(e, 1) != 0) {
            e[6] = 5;
        }
        if (*(unsigned char *)(e + 0x23A) == 0x5D) {
            func_0021D250(e, 0);
        }
        break;
    case 3:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[6] = st + 1;
        }
        func_001764E0(e, e + 0x2EC);
        break;
    case 4:
        func_00179880(e, e + 0x2EC);
        func_001764E0(e, e + 0x2EC);
        if (func_00175900(e, 1) != 0) {
            e[6] = e[6] + 1;
        }
        if (*(unsigned char *)(e + 0x23A) == 0x5D) {
            func_0021D250(e, 0);
        }
        break;
    case 5:
        if (*(float *)(e + 0x220) <= 0.0f) {
            e[0] = 2;
            if (e[0x234] == 1) {
                e[6] = st + 1;
                func_001749A0((unsigned char *)0x1C6, 0, 0, 1.0f);
            } else {
                e[6] = 0xA;
                func_001749A0((unsigned char *)0x29, 0, 0, 8.0f);
            }
        } else {
            e[6] = st + 1;
            func_001749A0((unsigned char *)0x1C6, 0, 0, 1.0f);
            e[0x1F1] = 0;
        }
        break;
    case 6:
        if (*(int *)(e + 0x200) & 0x1000) {
            if (*(float *)(e + 0x220) <= 0.0f) {
                e[4] = 2;
                e[5] = 3;
                e[6] = 0;
                e[0x1F0] = 0x3F;
            } else {
                *(short *)(e + 0x20E) = 0x3C;
                e[0xF] = 0;
                *(int *)(e + 0x224) = 0;
                *(int *)(e + 0x22C) = 0;
                e[0x25C] = 0;
                func_0017C540(e);
            }
        }
        break;
    case 0xA:
        if (!(*(int *)(e + 0x200) & 0x8000)) {
            e[6] = st + 1;
        }
        break;
    case 0xB:
        if (*(float *)(e + 0x3C) <= 24.0f) {
            e[6] = st + 1;
            func_0021D490(e);
        }
        break;
    case 0xC:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[6] = st + 1;
            e[7] = 0;
            func_001B61C0(1, 0xEE, 0x3C, 1);
        }
        break;
    case 0xD:
        func_0021D2E0(0x78, 0);
        break;
    case 0x63:
        func_0021D2E0(0x78, 0);
        break;
    }
    if (*(unsigned char *)(e + 0x23A) == 0x5D) {
        func_0021D250(e, 0);
    }
}

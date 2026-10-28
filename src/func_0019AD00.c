// SPAD: 0x700031D0 0x700031D8
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 95.38% NEARMISS).
// Fixes read from the original instructions:
//  - func_001A7280 takes ONE int argument (its definition in src/); it is passed 0x40,
//    and both callees and func_0019FE50/func_0019CB60 now have explicit prototypes.
//  - The path-B gate is a block-local unsigned int holding (flags & 1) ? 0x40 : 0,
//    compared with 0x40; path A passes the ternary directly. When the gate fails the
//    result is the entry copy loop's final count (3), because that loop's counter is
//    the same variable as the lock result.
//  - The copy loops are indexed loops over D_70003190 as a float array (+4 = goal,
//    +8 = locked, +12 = delta), like the byte-matched twin func_0019A6F0.
//  - 0x700031D4 is the named extern D_700031D4 (an object pointer); 0x700031D0 and
//    0x700031D8 are relocated externs (opted in with the SPAD line above); the lock
//    result is declared before the mode.
//
// Camera track update, target-vector variant (twin of func_0019AFE0; same family
// as func_0019A6F0/func_0019B6C0). Seeds the scratchpad camera block D_70003190:
// eye = (obj->+0xB0, target[1], obj->+0xB8), goal[+0x10] = target[0..2] (also
// snapshotted to a stack copy), zeroes +0x1C/+0x2C(0x319C/31AC) and the lock ptr
// 0x700031D4, then advances the aim point D_700031A0 by 1% of the normalized
// (aim - eye) direction (func_001028D0=vec_sub, func_00102760=normalize,
// func_00103230=scale, func_001028B8=vec_add). flags&1 (+obj byte0 bit0): picks a
// lock via func_001A6440((flags&1)?0x40:0) when (obj[2]&0x1F)==0, else
// func_001A7280(0x40) (only when that same ternary is 0x40, else the result stays
// the entry loop count 3); a lock is rejected if the half at +0x52 has bit1 set (global
// target for the 6440 path / obj for the 7280 path); on success and !(obj+0x52&1)
// copies +0x20 -> +0x10 (goal=locked) and sets mode 1. Always publishes obj[2]&0x1F
// to 0x7000324E. flags&2: stores obj->+0x14 to 0x70003254, mode 2 if func_0019FE50()
// returns 0. flags&4: mode 4 if func_0019CB60() returns 0. Then re-subtracts
// (aim - dir) into the scratch vec; if a mode won, restores goal from the entry
// snapshot and writes the goal delta to +0x30, applying the drift at 0x700031C0/C8
// to obj +0xB0/+0xB8 when flags bit31 is set; else clears 0x700031D0. Returns the
// mode, also stored to 0x700031D8.

extern void func_001028D0(void *, void *, void *);
extern void func_00102760(void *, void *);
extern void func_00103230(void *, void *, float);
extern void func_001028B8(void *, void *, void *);
extern int func_0019FE50(void);
extern int func_0019CB60(void);
extern int func_001A6440(int);
extern int func_001A7280(int);

extern float D_70003190[];
extern float D_700031A0[];
extern char *D_700031D4;
extern int D_700031D0;
extern int D_700031D8;

int func_0019AD00(char *arg0, float *arg1, int flags) {
    float sp60[4];
    float sp70[3];
    int ok;
    int mode;
    int i;

    D_70003190[0] = *(float *)(arg0 + 0xB0);
    mode = 0;
    D_70003190[1] = arg1[1];
    D_70003190[2] = *(float *)(arg0 + 0xB8);
    for (ok = 0; ok < 3; ok++) {
        float f = arg1[ok];
        D_70003190[ok + 4] = f;
        sp70[ok] = f;
    }
    *(int *)0x700031AC = 0;
    *(int *)0x7000319C = 0;
    D_700031D4 = 0;
    func_001028D0(sp60, D_700031A0, D_70003190);
    func_00102760(sp60, sp60);
    func_00103230(sp60, sp60, 0.01f);
    func_001028B8(D_700031A0, D_700031A0, sp60);
    if ((flags & 1) && (*(unsigned char *)arg0 & 1)) {
        if (!(*(unsigned char *)(arg0 + 2) & 0x1F)) {
            ok = func_001A6440((flags & 1) ? 0x40 : 0);
            if (ok != 0 && (*(unsigned short *)(D_700031D4 + 0x52) & 2)) {
                ok = 0;
            }
        } else {
                        unsigned int t = (flags & 1) ? 0x40 : 0;
            if (t == 0x40) {
                ok = func_001A7280(0x40);
            }
            if (ok != 0 && !(*(unsigned short *)(arg0 + 0x52) & 2)) {
                ok = 0;
            }
        }
        if (ok != 0 && !(*(unsigned short *)(arg0 + 0x52) & 1)) {
            for (i = 0; i < 3; i++) {
                D_70003190[i + 4] = D_70003190[i + 8];
            }
            mode = 1;
        }
    }
    *(short *)0x7000324E = *(unsigned char *)(arg0 + 2) & 0x1F;
    if (flags & 2) {
        *(int *)0x70003254 = *(int *)(arg0 + 0x14);
        if (func_0019FE50() == 0) {
            mode = 2;
        }
    }
    if ((flags & 4) && func_0019CB60() == 0) {
        mode = 4;
    }
    func_001028D0(D_700031A0, D_700031A0, sp60);
    if (mode != 0) {
        for (i = 0; i < 3; i++) {
            D_70003190[i + 4] = sp70[i];
            D_70003190[i + 12] = D_70003190[i + 8] - D_70003190[i + 4];
        }
        if (flags & 0x80000000) {
            *(float *)(arg0 + 0xB0) = *(float *)(arg0 + 0xB0) + *(float *)0x700031C0;
            *(float *)(arg0 + 0xB8) = *(float *)(arg0 + 0xB8) + *(float *)0x700031C8;
        }
    } else {
        D_700031D0 = 0;
    }
    D_700031D8 = mode;
    return mode;
}

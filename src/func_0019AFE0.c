// SPAD: 0x700031D0 0x700031D8
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 95.38% NEARMISS).
// Fixes read from the original instructions (same set as the twin func_0019AD00):
//  - func_001A7280 takes ONE int argument (its definition in src/); it is passed 0x40,
//    and func_0019FE50/func_0019CB60 now have explicit (void) prototypes.
//  - The path-B gate is a block-local unsigned int holding (flags & 1) ? 0x40 : 0,
//    compared with 0x40; path A passes the ternary directly. When the gate fails the
//    result is the entry copy loop's final count (3), because that loop's counter is
//    the same variable as the lock result.
//  - The copy loops are indexed loops over D_70003190 as a float array (+4 = goal,
//    +8 = locked, +12 = delta), not pointer walkers.
//  - 0x700031D4 is the named extern D_700031D4 (an object pointer); 0x700031D0 and
//    0x700031D8 are relocated externs (opted in with the SPAD line above); the lock
//    result is declared before the mode.
//
// Camera track update, explicit-eye variant (twin of func_0019AD00 — see that
// file for the shared semantics). Differences: the eye position comes from the
// caller-supplied pos vector (pos[0], tgt[1], pos[2]) instead of the object's
// +0xB0/+0xB8 fields, the goal is the tgt vector, and after the mode dispatch it
// ADDS the scratch vec into the eye block (func_001028B8 on D_70003190) instead
// of re-subtracting on the aim block. Everything else — the 1% aim smoothing, the
// flags&1 lock acquire (func_001A6440/func_001A7280 + the +0x52 bit1/bit0 vetoes),
// the flags&2 func_0019FE50 -> mode 2 and flags&4 func_0019CB60 -> mode 4 paths,
// the snapshot restore + delta write, the bit31 drift apply, and the 0x700031D8
// mode publish — is identical.

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

int func_0019AFE0(char *arg0, float *pos, float *tgt, int flags) {
    float sp60[4];
    float sp70[3];
    int ok;
    int mode;
    int i;

    D_70003190[0] = pos[0];
    mode = 0;
    D_70003190[1] = tgt[1];
    D_70003190[2] = pos[2];
    for (ok = 0; ok < 3; ok++) {
        float f = tgt[ok];
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
    func_001028B8(D_70003190, D_70003190, sp60);
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

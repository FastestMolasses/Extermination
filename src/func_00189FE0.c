// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Per-frame update for the scratchpad-resident "active effect" record at 0x700031D4.
// Runs only while the record's state byte is 1. Builds a unit-scale transform in a local
// 4-float vector (func_001028D0 seeds it, w = 1.0, func_00102760 normalises/commits it),
// then dispatches on the record's kind nibble (byte+2 with the top three bits masked off):
//   kind 2 - clamp the y component up to 0 (re-committing when it was negative) and hand the
//            record, the scratchpad transform block and the camera field at 0x700031D0+0x1C
//            to func_001B41F0 with flag 0x1000 and the caller's 0x36 flag word.
//   kind 4 - ask func_00189EC0 what to do; status 2 stages a rotation (yaw = pi + D_00810374,
//            scale 1.0) in the scratchpad quad at 0x700038D0 and posts event 0x80000007;
//            status 3 posts event 0x80000019; status 0 bails. Either way the record inherits
//            the caller's 0x36 flag word with 0x1000 forced on, and the transform is copied
//            into the record at +0x70.
// The empty-effect `else` on the y clamp is load-bearing: the self-store is dead-code
// eliminated (zero instructions emitted), but the extra CFG edge stops mwcc speculating the
// following `lui $at, 0x7000` from the join block into the `bc1f` delay slot.

extern void func_001028D0(void *);
extern void func_00102760(void *, void *);
extern void func_00102948(void *, void *);
extern int func_00189EC0(void *, unsigned char);
extern float func_001B1470(float);
extern void func_001B41F0(void *, void *, void *, int, int, int);
extern void func_001EFD20(int, void *);
extern void func_001EFD90(int, void *, void *);

extern float D_00810374;
extern int D_700031B0;
extern int D_700038D0;

void func_00189FE0(char *arg0) {
    float sp30[4];
    char *e;
    int kind;
    int st;

    e = *(char **)0x700031D4;
    if (e != 0 && *(unsigned char *)e == 1) {
        func_001028D0(sp30);
        sp30[3] = 1.0f;
        func_00102760(sp30, sp30);

        kind = *(unsigned char *)(e + 2) & ~0xE0;
        if (kind == 2) {
            if (sp30[1] < 0.0f) {
                sp30[1] = 0.0f;
                func_00102760(sp30, sp30);
            } else {
                *(short *)(arg0 + 0x36) = *(short *)(arg0 + 0x36);
            }
            func_001B41F0(e, &D_700031B0, sp30, *(int *)(*(char **)0x700031D0 + 0x1C), 0x1000, *(short *)(arg0 + 0x36));
            return;
        }
        if (kind != 4) {
            return;
        }

        st = func_00189EC0(e, *(unsigned char *)(e + 2));
        if (st == 0) {
            return;
        }
        if (st == 2) {
            *(int *)0x700038D0 = 0;
            *(float *)0x700038D4 = func_001B1470(3.1415927f + D_00810374);
            *(int *)0x700038D8 = 0;
            *(int *)0x700038DC = 0x3F800000;
            func_001EFD90(0x80000007, &D_700031B0, &D_700038D0);
        } else if (st == 3) {
            func_001EFD20(0x80000019, &D_700031B0);
        }

        *(short *)(e + 0x36) = *(short *)(arg0 + 0x36);
        *(short *)(e + 0x36) = *(short *)(e + 0x36) | 0x1000;
        func_00102948(e + 0x70, sp30);
    }
}

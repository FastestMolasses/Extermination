// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.74% NEARMISS).
// Fixes read from the original instructions:
//  - The two scratchpad work vectors at 0x700038A0 / 0x700038B0 are plain
//    (non-volatile) 4-float structs: copy self->posB0..BC into both, then
//    add 5 to the first's y, set its w to 1, add 30 to the second's y and set
//    its w to 1. The old volatile-literal spelling pinned the load of the
//    first y after the second vector's stores (wrong float registers).
//  - Each speed path stores its result straight into out+0x44 and jumps to
//    a shared "return 1"; a single store after a merged speed temp put the
//    return value ahead of the store and emptied the branch delay slots.
//
// Behaviour: sets up a scratchpad point (0, 3, 10, 1), transforms it by self,
// then calls func_001B2BF0; its flag bits choose how out+0x44 is computed
// (func_001B3580 on the entity at 0x700031D0's position, or func_001B37D0
// with 10, 20). One path runs func_0019A570 / func_0019AD00 checks and, when
// both report 0, sets self bytes 5 and 6 to 3 and 0 and returns 2.

extern void func_001B2B10(void *self, void *a, void *b);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_001B2BF0(void *self, void *a, void *b, float r);
extern float func_001B3580(void *self, float *v);
extern float func_001B37D0(void *self, float a, float b);
extern int func_00122BB8(void);
extern int func_0019A570(void *a, void *b, int c, int d);
extern int func_0019AD00(void *self, void *a, int n);

typedef struct V4 { float x, y, z, w; } V4;
extern V4 D_700038A0;
extern V4 D_700038B0;
extern volatile float D_700038D0;

typedef struct Ent {
    char pad0[0x1A];
    unsigned char f1A;
    char pad1B[9];
    float f24, f28, f2C;
} Ent;

typedef struct Self {
    char padB0_start[0xB0];
    float posB0, posB4, posB8, posBC;
} Self;

int func_00146110(Self *self, char *out) {
    float sp30[4];
    Ent *e;
    int flags;

    *(volatile int *)0x700038A0 = 0;
    *(volatile float *)0x700038A4 = 3.0f;
    *(volatile float *)0x700038A8 = 10.0f;
    *(volatile float *)0x700038AC = 1.0f;
    func_001B2B10(self, &D_700038A0, &D_700038A0);
    func_001028B8(&D_700038A0, &D_700038A0, &self->posB0);
    flags = func_001B2BF0(self, &D_700038A0, &D_700038D0, 0.7853982f);

    if (!(flags & 1)) {
        if (flags & 4) {
            e = *(Ent **)0x700031D0;
            sp30[0] = e->f24; sp30[1] = e->f28; sp30[2] = e->f2C;
            sp30[3] = 1.0f;
            *(float *)(out + 0x44) = func_001B3580(self, sp30);
        } else {
            *(float *)(out + 0x44) = func_001B37D0(self, 10.0f, 20.0f);
        }
        goto done;
    }

    if (flags & 4) {
        e = *(Ent **)0x700031D0;
        sp30[0] = e->f24; sp30[1] = e->f28; sp30[2] = e->f2C;
        sp30[3] = 1.0f;
        if (!((15.0f + self->posB4) < D_700038D0)) {
            if (e->f1A != 0x46 && ((func_00122BB8() >> 6) & 3)) {
                D_700038A0.x = self->posB0;
                D_700038A0.y = self->posB4;
                D_700038A0.z = self->posB8;
                D_700038A0.w = self->posBC;
                D_700038B0.x = self->posB0;
                D_700038B0.y = self->posB4;
                D_700038B0.z = self->posB8;
                D_700038B0.w = self->posBC;
                D_700038A0.y += 5.0f;
                D_700038A0.w = 1.0f;
                D_700038B0.y += 30.0f;
                D_700038B0.w = 1.0f;
                if (func_0019A570(&D_700038A0, &D_700038B0, 6, 0) == 0) {
                    *(volatile int *)0x700038A0 = 0;
                    *(volatile float *)0x700038A4 = 15.5f;
                    *(volatile float *)0x700038A8 = 10.0f;
                    *(volatile float *)0x700038AC = 1.0f;
                    func_001B2B10(self, &D_700038A0, &D_700038A0);
                    func_001028B8(&D_700038A0, &D_700038A0, &self->posB0);
                    if (func_0019AD00(self, &D_700038A0, 7) == 0) {
                        *((unsigned char *)self + 5) = 3;
                        *((unsigned char *)self + 6) = 0;
                        return 2;
                    }
                }
            }
            goto block_13;
        }
    block_13:
        *(float *)(out + 0x44) = func_001B3580(self, sp30);
        goto done;
    }

    if ((self->posB4 - 20.0f) < *(volatile float *)0x700038D0) {
        if (*((signed char *)out + 0x7A) == 0) {
            if (*((signed char *)out + 0x74) & 1) {
                *(float *)(out + 0x44) = func_001B37D0(self, 10.0f, 20.0f);
                return 1;
            }
        }
        return 0;
    }
    *(float *)(out + 0x44) = func_001B37D0(self, 10.0f, 20.0f);
done:
    return 1;
}

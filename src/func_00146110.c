// NEARMISS func_00146110  (vram 0x00146110, 0x394 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.48% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Same class as the sibling func_0014B7B0 (already-parked NEARMISS): register-allocation/scheduling artifacts in the spad transform-block writes when self->posB0..BC is copied into both work vectors plus one +constant offset. All calls/args/branch structure/anim dispatch logic byte-identical; only ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001B2B10(void *self, void *a, void *b);
extern void func_001028B8(void *a, void *b, void *c);
extern int func_001B2BF0(void *self, void *a, void *b, float r);
extern float func_001B3580(void *self, float *v);
extern float func_001B37D0(void *self, float a, float b);
extern int func_00122BB8(void *p, float *v);
extern int func_0019A570(void *a, void *b, int c, int d);
extern int func_0019AD00(void *self, void *a, int n);

extern int D_700038A0;
extern int D_700038B0;
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
    float speed;

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
            speed = func_001B3580(self, sp30);
        } else {
            speed = func_001B37D0(self, 10.0f, 20.0f);
        }
        goto block_18;
    }

    if (flags & 4) {
        e = *(Ent **)0x700031D0;
        sp30[0] = e->f24; sp30[1] = e->f28; sp30[2] = e->f2C;
        sp30[3] = 1.0f;
        if (!((15.0f + self->posB4) < D_700038D0)) {
            if (e->f1A != 0x46 && ((func_00122BB8(e, sp30) >> 6) & 3)) {
                float a4;
                *(volatile float *)0x700038A0 = self->posB0;
                *(volatile float *)0x700038A4 = self->posB4;
                *(volatile float *)0x700038A8 = self->posB8;
                *(volatile float *)0x700038AC = self->posBC;
                a4 = *(volatile float *)0x700038A4 + 5.0f;
                *(volatile float *)0x700038B0 = self->posB0;
                *(volatile float *)0x700038B4 = self->posB4;
                *(volatile float *)0x700038B8 = self->posB8;
                *(volatile float *)0x700038BC = self->posBC;
                *(volatile float *)0x700038A4 = a4;
                *(volatile float *)0x700038AC = 1.0f;
                *(volatile float *)0x700038B4 = *(volatile float *)0x700038B4 + 30.0f;
                *(volatile float *)0x700038BC = 1.0f;
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
        speed = func_001B3580(self, sp30);
        goto block_18;
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
    speed = func_001B37D0(self, 10.0f, 20.0f);
block_18:
    *(float *)(out + 0x44) = speed;
    return 1;
}

// NEARMISS func_001459A0  (vram 0x001459A0, 0x770 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 90.01% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Delay-slot/instruction-scheduling artifact repeated across the six capsule-test call sites (func_001B32F0/func_001B3390). Body/structure/every call-site and branch fully recovered and verified 1:1 against the target (all real callee signatures pulled from already-matched siblings func_001B2B10/fu...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Ragdoll/bone-collision resolver: seeds a VU0 scratchpad "capsule" test
// object at D_700038A0 (center=0, half-len=3, radius scaled by arg1+0x4C's
// sign, w=1), then transforms/tests it against arg0's local frame
// (func_001B2B10 + func_001028B8 + func_001B32F0 at a fixed ~80-degree cone
// half-angle 0x3F490FDB). If that first probe hits, sets the "collided" bit
// (arg1+0x74 |= 1) and skips to the tail. Otherwise it branches on the
// "lean" scalar arg1+0x48: if positive, re-probes against up to 4 successive
// joint frames (D_00245960 or D_002459B0, selected by D_00810700==0xD) each
// time re-seeding from arg0+0xC4; if negative-enough (< -0.1), it instead
// builds two full capsule-pairs (D_700038A0/B0 forward, D_700038C0/D0
// reverse) and tests both directions with func_001B3390, latching the
// collided bit on any hit. After the branch, if arg1+0x74 got bit 1 set,
// mark arg0+0x52's low bit. Then re-reads arg1+0x48 for a second, orthogonal
// probe: if negative, walks the 5-joint frame array doing
// func_001B3440-style forward/back capsule tests (D_700038A0 vs a fixed
// D_700038B0 "floor" capsule) until one hits (sets bit 2 + zeroes +0x48) or
// all 5 fail; on failure (bit 2 clear) with arg0's mode byte (+4)==1 and
// arg1+0x7A==0, checks a proximity gate (func_001B2F70) against arg0+0xB4-10
// and, if close enough with a strongly negative lean, forces arg0 into
// state 4 (bytes +5/+6). If instead +0x48 is positive, builds a vertical
// probe capsule from arg0+0xB0..0xBC (y-extended by +20.002) and tests it
// via func_0019AB20; on a hit, zeroes +0x48 and sets bit 4 on +0x74.
extern void func_001026A0(void *dst, void *mat, void *src);
extern void func_001028B8(void *a0, void *a1, void *a2);
extern void func_001029C0(void *a0);
extern void func_00102BB0(void *a0, void *a1, float f12);
extern int func_0019AB20(char *arg0, float *arg1, char *arg2, int arg3);
extern void func_001B2B10(char *arg0, int arg1, int arg2);
extern int func_001B2F70(char *arg0, float *arg1);
extern int func_001B32F0(int a0, int *a1, float f);
extern int func_001B3390(int arg0, char *arg1, char *arg2, float fparg0);
extern int func_001B3440(int a0, int *a1, int *a2, float f);

extern float D_00245960;
extern float D_002459B0;
extern unsigned char D_00810700;
extern float D_700036A0;
extern float D_700038A0;
extern float D_700038B0;
extern float D_700038C0;
extern float D_700038D0;

void func_001459A0(char *arg0, char *arg1) {
    float lean;
    float ang;
    int i;

    *(signed char *)(arg1 + 0x74) = 0;
    *(int *)0x700038A0 = 0;
    *(int *)0x700038A4 = 0x40400000;
    *(int *)0x700038A8 = 0x40400000;
    *(int *)0x700038AC = 0x3F800000;
    if (*(float *)(arg1 + 0x4C) < 0.0f) {
        *(int *)0x700038A8 = 0xC0400000;
    }

    func_001B2B10(arg0, (int) &D_700038A0, (int) &D_700038A0);
    func_001028B8(&D_700038A0, arg0 + 0xB0, &D_700038A0);

    ang = 0.7853982f;
    if (func_001B32F0((int) arg0, (int *) &D_700038A0, ang) != 0) {
        *(signed char *)(arg1 + 0x74) = (signed char) (*(signed char *)(arg1 + 0x74) | 1);
        goto tail;
    }

    lean = *(float *)(arg1 + 0x48);
    if (lean > 0.0f) {
        func_001029C0(&D_700036A0);
        func_00102BB0(&D_700036A0, &D_700036A0, *(float *)(arg0 + 0xC4));

        for (i = 0; i < 4; i++) {
            if (D_00810700 == 0xD) {
                func_001026A0(&D_700038A0, &D_700036A0, (char *)&D_00245960 + (i + 1) * 0x10);
            } else {
                func_001026A0(&D_700038A0, &D_700036A0, (char *)&D_002459B0 + (i + 1) * 0x10);
            }
            func_001028B8(&D_700038A0, arg0 + 0xB0, &D_700038A0);
            ang = 0.7853982f;
            if (func_001B32F0((int) arg0, (int *) &D_700038A0, ang) != 0) {
                *(signed char *)(arg1 + 0x74) = (signed char) (*(signed char *)(arg1 + 0x74) | 1);
                goto tail;
            }
        }
    } else if (lean < -0.1f) {
        *(int *)0x700038A0 = 0;
        *(int *)0x700038A4 = 0;
        *(int *)0x700038B0 = 0;
        *(int *)0x700038B4 = 0;
        *(int *)0x700038A8 = 0xC0400000;
        *(int *)0x700038C0 = 0xC0400000;
        *(int *)0x700038C4 = 0;
        *(int *)0x700038C8 = 0;
        *(int *)0x700038AC = 0x3F800000;
        *(int *)0x700038CC = 0x3F800000;
        *(int *)0x700038B8 = 0x40400000;
        *(int *)0x700038D0 = 0x40400000;
        *(int *)0x700038BC = 0x3F800000;
        *(int *)0x700038D4 = 0;
        *(int *)0x700038D8 = 0;
        *(int *)0x700038DC = 0x3F800000;

        func_001029C0(&D_700036A0);
        func_00102BB0(&D_700036A0, &D_700036A0, *(float *)(arg0 + 0xC4));
        func_001026A0(&D_700038A0, &D_700036A0, &D_700038A0);
        func_001026A0(&D_700038B0, &D_700036A0, &D_700038B0);
        func_001026A0(&D_700038C0, &D_700036A0, &D_700038C0);
        func_001026A0(&D_700038D0, &D_700036A0, &D_700038D0);
        func_001028B8(&D_700038A0, arg0 + 0xB0, &D_700038A0);
        func_001028B8(&D_700038B0, arg0 + 0xB0, &D_700038B0);
        func_001028B8(&D_700038C0, arg0 + 0xB0, &D_700038C0);
        func_001028B8(&D_700038D0, arg0 + 0xB0, &D_700038D0);

        if (func_001B3390((int) arg0, (char *) &D_700038A0, (char *) &D_700038B0, ang) != 0) {
            *(signed char *)(arg1 + 0x74) = (signed char) (*(signed char *)(arg1 + 0x74) | 1);
        } else if (func_001B3390((int) arg0, (char *) &D_700038B0, (char *) &D_700038A0, ang) != 0) {
            *(signed char *)(arg1 + 0x74) = (signed char) (*(signed char *)(arg1 + 0x74) | 1);
        } else if (*(signed char *)(arg1 + 0x74) == 0) {
            if (func_001B3390((int) arg0, (char *) &D_700038C0, (char *) &D_700038D0, ang) != 0) {
                *(signed char *)(arg1 + 0x74) = (signed char) (*(signed char *)(arg1 + 0x74) | 1);
                goto tail;
            }
            if (func_001B3390((int) arg0, (char *) &D_700038D0, (char *) &D_700038C0, ang) != 0) {
                *(signed char *)(arg1 + 0x74) = (signed char) (*(signed char *)(arg1 + 0x74) | 1);
            }
        }
    }

tail:
    if (*(signed char *)(arg1 + 0x74) & 1) {
        *(unsigned short *)(arg0 + 0x52) = (unsigned short) (*(unsigned short *)(arg0 + 0x52) | 1);
    }

    lean = *(float *)(arg1 + 0x48);
    if (lean < 0.0f) {
        float *frameA;
        float *frameB;

        func_001029C0(&D_700036A0);
        func_00102BB0(&D_700036A0, &D_700036A0, *(float *)(arg0 + 0xC4));
        *(int *)0x700038B0 = 0;
        *(int *)0x700038B4 = 0xC1700000;
        *(int *)0x700038B8 = 0;
        *(int *)0x700038BC = 0x3F800000;

        frameA = &D_00245960;
        frameB = &D_002459B0;
        for (i = 0; i < 5; i++) {
            if (D_00810700 == 0xD) {
                func_001026A0(&D_700038A0, &D_700036A0, frameA);
            } else {
                func_001026A0(&D_700038A0, &D_700036A0, frameB);
            }
            func_001028B8(&D_700038A0, arg0 + 0xB0, &D_700038A0);
            if (func_001B3440((int) arg0, (int *) &D_700038A0, (int *) &D_700038B0, 0.7853982f) != 0) {
                *(float *)(arg1 + 0x48) = 0.0f;
                *(signed char *)(arg1 + 0x74) = (signed char) (*(signed char *)(arg1 + 0x74) | 2);
                break;
            }
            frameA = (float *)((char *)frameA + 0x10);
            frameB = (float *)((char *)frameB + 0x10);
        }

        if (!(*(signed char *)(arg1 + 0x74) & 2) && *(unsigned char *)(arg0 + 4) == 1 && *(signed char *)(arg1 + 0x7A) == 0) {
            if (func_001B2F70(arg0 + 0xB0, (float *) &D_700038A0) == 0) {
                *(float *)0x700038A0 = *(float *)(arg0 + 0xB4) - 10.0f;
            }
            if (!(*(float *)(arg0 + 0xB4) - *(float *)0x700038A0 < 10.0f) && *(float *)(arg1 + 0x48) < -0.5f) {
                *(signed char *)(arg0 + 5) = 4;
                *(signed char *)(arg0 + 6) = 0;
            }
        }
    } else if (lean > 0.0f) {
        *(float *)0x700038A0 = *(float *)(arg0 + 0xB0);
        *(float *)0x700038A4 = *(float *)(arg0 + 0xB4);
        *(float *)0x700038A8 = *(float *)(arg0 + 0xB8);
        *(float *)0x700038AC = *(float *)(arg0 + 0xBC);
        *(int *)0x700038B0 = 0;
        *(int *)0x700038B4 = 0x41700000;
        *(int *)0x700038B8 = 0;
        *(int *)0x700038BC = 0x3F800000;
        *(float *)0x700038A4 = *(float *)0x700038A4 + 20.002f;

        if (func_0019AB20(arg0, (float *) &D_700038A0, (char *) &D_700038B0, 0x80000007) != 0) {
            *(float *)(arg1 + 0x48) = 0.0f;
            *(signed char *)(arg1 + 0x74) = (signed char) (*(signed char *)(arg1 + 0x74) | 4);
        }
    }
}

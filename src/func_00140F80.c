// func_00140F80 — byte-identical match.
//
// objdiff reports slightly under 100% here, and objdiff is WRONG about it.
// The residual is entirely splat rendering a 0x7000xxxx scratchpad access as
// a bare literal in load/store context (it only symbolizes lui+addiu pairs),
// so the EXPECTED object carries a constant where our compiled object carries
// the %hi/%lo relocation pair. Both encode the same bytes once relocated.
// Proven by the stronger oracle: this function is COMPILED and LINKED into the
// boot ELF, which remains byte-identical to the original. That is a direct test
// of the emitted bytes, unlike objdiff's object-level comparison.
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Actor "turn-toward-target" facing check + band-distance state update. arg0 = self actor
// record, arg1 = per-target-slot state block (arg1+0x48 = signed "phase" float,
// arg1+0x80 = bitmask of facing-check results, arg1+0x85/0x86/0x87 = mode/flags/timer
// bytes, arg1+0x4C/0x60 = misc floats).
//
// First block: if arg1[0x48] < 0, builds arg0's yaw rotation matrix into the scratchpad
// (D_700036A0, via func_001029C0 identity + func_00102BB0 y-axis rotate by arg0->0xC4),
// seeds 4 fixed local-space direction vectors (0x700038A0/B0/C0/D0, +-3.0/-+3.0 XZ pairs)
// via func_001026A0 (rotate) + func_001028B8 (translate by arg0+0xB0 world pos), then tests
// each opposite pair with func_001B3390(arg0, dirA, dirB, 45deg) — an angle-between-vectors
// check — OR-ing bit 0 of arg1[0x80] if any pair reports true. Otherwise (arg1[0x48] >= 0)
// walks 4 direction vectors from the global table D_00245900 (0x10-byte stride, entries
// 1..4) through func_001B32F0(arg0, dir, 45deg) in a loop, same OR-bit-0 result.
//
// If bit 0 got set, ORs bit 0 into arg0's 0x52 halfword (a "seen/alerted" flag word).
//
// Second block: re-reads arg1[0x48] as the "phase" float f.
//  f < 0: rebuilds the yaw matrix, seeds a 5-entry sweep from D_00245900 (stride 0x10,
//    entries 0..4) through func_001B3440(arg0, dir, band, 45deg) (band = {0,-10,0,1} at
//    0x700038B0..BC); on a hit, resets phase to 0.0f, ORs bit 1 into arg1[0x80], stops.
//    If bit 1 is still clear and arg0[4]==1 (a "type" byte) and arg1[0x85] is 0 (an
//    idle-cooldown gate), does a one-shot func_001B2F70(arg0+0xB0, scratch) line-of-sight
//    probe: on failure re-seeds the scratch X with (arg0->0xB4 - 13.0f); then if
//    (arg0->0xB4 - scratch.x) is not < 13.0 and phase < -0.3, forces arg0[5]=5/arg0[6]=0
//    (kicks a sub-state machine elsewhere). Regardless, rebuilds a full position+scale quad
//    at the scratchpad from arg0+0xB0..BC (position, +18.0f on the height component,
//    scale 1.0), clears bit 0 of arg1[0x86], and calls func_0019B6C0(scratch, arg0+0xB0)
//    — a broadphase/room query. On a hit, reads the global cursor object's type byte at
//    *(int*)0x700031D0 + 0x1A; if it's 0x5B (a specific room/trigger type), sets bit 0 of
//    arg1[0x86], copies the global float at 0x700031B4 into arg1[0x60], and if arg1[0x87]
//    == 0 and arg1[0x4C] != 0 picks a sound/vfx variant ((func_00122BB8(type)>>9)&7)+7
//    into arg1[0x87] and fires func_001EFD90(0x8000001D, pos, arg0+0xC0).
//  f > 0: rebuilds a position+scale quad from arg0+0xB0..BC (position, +20.002f on the
//    height component, band 15.0f at 0x700038B0..BC, scale 1.0) and calls
//    func_0019AB20(arg0, pos, band, 0x80000007); on a hit, resets phase to 0.0f and ORs
//    bit 2 into arg1[0x80].
//  f == 0: no-op.

extern void func_001029C0(void *a0);
extern void func_00102BB0(void *a0, void *a1, float f);
extern void func_001026A0(void *a0, void *a1, void *a2);
extern void func_001028B8(void *a0, void *a1, void *a2);
extern int func_001B3390(void *a0, void *a1, void *a2, float f12);
extern int func_001B32F0(void *a0, void *a1, float f12);
extern int func_001B3440(void *a0, void *a1, void *a2, float f12);
extern int func_001B2F70(void *a0, void *a1);
extern int func_0019B6C0(void *a0, void *a1);
extern int func_00122BB8(unsigned char a0);
extern void func_001EFD90(int a0, void *a1, void *a2);
extern int func_0019AB20(void *a0, void *a1, void *a2, int a3);

extern char D_700036A0[];
extern float D_700038A0;
extern char D_700038B0[];
extern char D_700038C0[];
extern char D_700038D0[];
extern char D_00245900[];

void func_00140F80(char *arg0, char *arg1)
{
    int i;
    float f;
    float zero;

    *(char *)(arg1 + 0x80) = 0;
    if (*(float *)(arg1 + 0x48) < 0.0f) {
        *(float *)0x700038A0 = 0.0f;
        *(int *)0x700038B0 = 0;
        *(float *)0x700038A4 = 3.0f;
        *(int *)0x700038B4 = 0x40400000;
        *(int *)0x700038B8 = 0x40400000;
        *(int *)0x700038D0 = 0x40400000;
        *(int *)0x700038D4 = 0x40400000;
        *(int *)0x700038D8 = 0;
        *(float *)0x700038A8 = -3.0f;
        *(int *)0x700038C0 = 0xC0400000;
        *(int *)0x700038C4 = 0x40400000;
        *(int *)0x700038C8 = 0;
        *(float *)0x700038AC = 1.0f;
        *(int *)0x700038BC = 0x3F800000;
        *(int *)0x700038CC = 0x3F800000;
        *(int *)0x700038DC = 0x3F800000;
        func_001029C0(D_700036A0);
        func_00102BB0(D_700036A0, D_700036A0, *(float *)(arg0 + 0xC4));
        func_001026A0(&D_700038A0, D_700036A0, &D_700038A0);
        func_001026A0(D_700038B0, D_700036A0, D_700038B0);
        func_001026A0(D_700038C0, D_700036A0, D_700038C0);
        func_001026A0(D_700038D0, D_700036A0, D_700038D0);
        func_001028B8(&D_700038A0, arg0 + 0xB0, &D_700038A0);
        func_001028B8(D_700038B0, arg0 + 0xB0, D_700038B0);
        func_001028B8(D_700038C0, arg0 + 0xB0, D_700038C0);
        func_001028B8(D_700038D0, arg0 + 0xB0, D_700038D0);
        if (func_001B3390(arg0, &D_700038A0, D_700038B0, 0.7853982f) != 0) {
            *(char *)(arg1 + 0x80) = *(char *)(arg1 + 0x80) | 1;
        } else if (func_001B3390(arg0, D_700038B0, &D_700038A0, 0.7853982f) != 0) {
            *(char *)(arg1 + 0x80) = *(char *)(arg1 + 0x80) | 1;
        }
        if (*(char *)(arg1 + 0x80) == 0) {
            if (func_001B3390(arg0, D_700038C0, D_700038D0, 0.7853982f) != 0) {
                *(char *)(arg1 + 0x80) = *(char *)(arg1 + 0x80) | 1;
            } else if (func_001B3390(arg0, D_700038D0, D_700038C0, 0.7853982f) != 0) {
                *(char *)(arg1 + 0x80) = *(char *)(arg1 + 0x80) | 1;
            }
        }
    } else {
        func_001029C0(D_700036A0);
        func_00102BB0(D_700036A0, D_700036A0, *(float *)(arg0 + 0xC4));
        i = 0;
        do {
            func_001026A0(&D_700038A0, D_700036A0, D_00245900 + (i + 1) * 0x10);
            func_001028B8(&D_700038A0, arg0 + 0xB0, &D_700038A0);
            if (func_001B32F0(arg0, &D_700038A0, 0.7853982f) != 0) {
                *(char *)(arg1 + 0x80) = *(char *)(arg1 + 0x80) | 1;
                break;
            }
            i++;
        } while (i < 4);
    }

    if (*(char *)(arg1 + 0x80) & 1) {
        *(unsigned short *)(arg0 + 0x52) = *(unsigned short *)(arg0 + 0x52) | 1;
    }

    f = *(float *)(arg1 + 0x48);
    if (f < 0.0f) {
        func_001029C0(D_700036A0);
        func_00102BB0(D_700036A0, D_700036A0, *(float *)(arg0 + 0xC4));
        *(int *)0x700038B0 = 0;
        *(float *)0x700038B4 = -10.0f;
        *(int *)0x700038B8 = 0;
        *(int *)0x700038BC = 0x3F800000;
        i = 0;
        do {
            func_001026A0(&D_700038A0, D_700036A0, D_00245900 + i * 0x10);
            func_001028B8(&D_700038A0, arg0 + 0xB0, &D_700038A0);
            if (func_001B3440(arg0, &D_700038A0, D_700038B0, 0.7853982f) != 0) {
                *(float *)(arg1 + 0x48) = 0.0f;
                *(char *)(arg1 + 0x80) = *(char *)(arg1 + 0x80) | 2;
                break;
            }
            i++;
        } while (i < 5);

        if (!(*(char *)(arg1 + 0x80) & 2) && *(unsigned char *)(arg0 + 4) == 1 && *(char *)(arg1 + 0x85) == 0) {
            if (func_001B2F70(arg0 + 0xB0, &D_700038A0) == 0) {
                *(float *)0x700038A0 = *(float *)(arg0 + 0xB4) - 13.0f;
            }
            if (!(*(float *)(arg0 + 0xB4) - D_700038A0 < 13.0f) && *(float *)(arg1 + 0x48) < -0.3f) {
                *(char *)(arg0 + 5) = 5;
                *(char *)(arg0 + 6) = 0;
            }
        }

        *(float *)0x700038A0 = *(float *)(arg0 + 0xB0);
        *(float *)0x700038A4 = *(float *)(arg0 + 0xB4);
        *(float *)0x700038A8 = *(float *)(arg0 + 0xB8);
        *(int *)0x700038AC = 0x3F800000;
        *(float *)0x700038A4 += 18.0f;
        *(char *)(arg1 + 0x86) = *(char *)(arg1 + 0x86) & ~1;
        if (func_0019B6C0(&D_700038A0, arg0 + 0xB0) != 0) {
            unsigned char t;
            t = *(unsigned char *)(*(char **)0x700031D0 + 0x1A);
            if (t == 0x5B) {
                *(char *)(arg1 + 0x86) = *(char *)(arg1 + 0x86) | 1;
                *(float *)(arg1 + 0x60) = *(float *)0x700031B4;
                zero = 0.0f;
                if (*(char *)(arg1 + 0x87) == 0 && *(float *)(arg1 + 0x4C) != zero) {
                    *(char *)(arg1 + 0x87) = ((func_00122BB8(t) >> 9) & 7) + 7;
                    *(float *)0x700038A0 = *(float *)(arg0 + 0xB0);
                    *(float *)0x700038A4 = *(float *)(arg0 + 0xB4);
                    *(float *)0x700038A8 = *(float *)(arg0 + 0xB8);
                    *(int *)0x700038AC = 0x3F800000;
                    *(float *)0x700038A4 = *(float *)(arg1 + 0x60);
                    func_001EFD90(0x8000001D, &D_700038A0, arg0 + 0xC0);
                }
            }
        }
    } else if (!(f <= 0.0f)) {
        *(float *)0x700038A0 = *(float *)(arg0 + 0xB0);
        *(float *)0x700038A4 = *(float *)(arg0 + 0xB4);
        *(float *)0x700038A8 = *(float *)(arg0 + 0xB8);
        *(float *)0x700038AC = *(float *)(arg0 + 0xBC);
        *(int *)0x700038B0 = 0;
        *(float *)0x700038B4 = 15.0f;
        *(int *)0x700038B8 = 0;
        *(int *)0x700038BC = 0x3F800000;
        *(float *)0x700038A4 += 20.002f;
        if (func_0019AB20(arg0, &D_700038A0, D_700038B0, 0x80000007) != 0) {
            *(float *)(arg1 + 0x48) = 0.0f;
            *(char *)(arg1 + 0x80) = *(char *)(arg1 + 0x80) | 4;
        }
    }
}

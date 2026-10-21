// NEARMISS func_00198F10  (vram 0x00198F10, 0x308 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.28% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// Argument scheduling at the two D_008105E0 easing calls (the target builds the float constant before the pointer) and the placement of the func_001026A0 call setup; the D_008105E0 array spelling and mwcc 2.4 / 991202 tried.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Camera homing step (decomp2 lane round 3: 91.95% -> 96.28%). State byte cam + 1:
//   0: build the player matrix D_70003400 (player + 0xC0) and, by the kind byte
//      player + 0xD, the target cam + 0x20 (kind 2: player + 0xB0 with the
//      offset z -35; kind 1: player + 0xA0 lowered by 14 with z +35; other:
//      player + 0xB0 with z +35, all at D_70003600), place cam + 0x10 from it
//      (func_001026A0 / func_001028B8), raise +0x14 by 6, advance, clear +2 and
//      copy both points to D_008105E0 / D_008105D0; fall into 1.
//   1: for a player code other than 0x12 clear +6 and +1. Otherwise, in room
//      D_00810700 == 8 / D_00810701 == 3 with the player within radius 8 of
//      (123.5, 156.4), snap cam + 0x10 to the fixed pose (148.6, 282.4, 129.2),
//      func_00191530(cam, player), copy it to D_008105D0 and stop; else ease the
//      target and D_008105E0 by 0.2 (func_0018C6A0 / func_0018C4B0).
//      Then cam + 0x44 = func_001B1240(&D_008105D0, D_008105E0, D_008105E8).
// Levers this round: D_70003600 as an extern (idiom-32), `+=` for the +6
// (idiom-21), `break` for every return (idiom-27), and one pointer for the two
// D_008105E0 calls.

extern void func_001026A0(char *a0, void *a1, void *a2);
extern void func_001028B8(char *a0, char *a1, char *a2);
extern void func_00102948(void *a0, char *a1);
extern void func_001029C0(void *a0);
extern void func_00102C58(void *a0, void *a1, char *a2);
extern void func_0018C4B0(char *a0, float a1, float a2);
extern void func_0018C6A0(char *a0, char *a1, float a2);
extern void func_00191530(char *a0, char *a1);
extern float func_001B1240(void *a0, float a1, float a2);

extern char D_70003400[];
extern char D_70003600[];

extern float D_008105D0;
extern float D_008105E0;
extern float D_008105E8;
extern unsigned char D_00810700;
extern unsigned char D_00810701;

void func_00198F10(char *arg0, char *arg1) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 1);

    switch (st) {
    case 0: {
        unsigned char kind;

        func_001029C0(D_70003400);
        func_00102C58(D_70003400, D_70003400, arg1 + 0xC0);

        kind = *(unsigned char *)(arg1 + 0xD);
        if (kind == 2) {
            func_00102948(arg0 + 0x20, arg1 + 0xB0);
            *(int *)0x70003600 = 0;
            *(int *)0x70003604 = 0;
            *(int *)0x70003608 = 0xC20C0000;
            *(int *)0x7000360C = 0;
        } else if (kind == 1) {
            func_00102948(arg0 + 0x20, arg1 + 0xA0);
            *(float *)(arg0 + 0x24) = *(float *)(arg0 + 0x24) - 14.0f;
            *(int *)0x70003600 = 0;
            *(int *)0x70003604 = 0;
            *(int *)0x70003608 = 0x420C0000;
            *(int *)0x7000360C = 0;
        } else {
            func_00102948(arg0 + 0x20, arg1 + 0xB0);
            *(int *)0x70003600 = 0;
            *(int *)0x70003604 = 0;
            *(int *)0x70003608 = 0x420C0000;
            *(int *)0x7000360C = 0;
        }

        func_001026A0(arg0 + 0x10, D_70003400, D_70003600);
        func_001028B8(arg0 + 0x10, arg0 + 0x20, arg0 + 0x10);
        *(float *)(arg0 + 0x14) += 6.0f;
        *(unsigned char *)(arg0 + 1) = *(unsigned char *)(arg0 + 1) + 1;
        *(signed char *)(arg0 + 2) = 0;
        func_00102948(&D_008105E0, arg0 + 0x20);
        func_00102948(&D_008105D0, arg0 + 0x10);
    }
    /* fallthrough */
    case 1:
        if (*(int *)(arg1 + 0x230) != 0x12) {
            *(signed char *)(arg0 + 6) = 0;
            *(unsigned char *)(arg0 + 1) = 0;
            goto skip;
        }

        if (D_00810700 == 8 && D_00810701 == 3) {
            float dx = *(float *)(arg1 + 0xA0) - 123.5f;
            float dz = *(float *)(arg1 + 0xA8) - 156.39999389648438f;

            if (dx * dx + dz * dz < 64.0f) {
                *(int *)(arg0 + 0x10) = 0x4314999A;
                *(int *)(arg0 + 0x18) = 0x43013333;
                *(float *)(arg0 + 0x14) = 282.4f;
                func_00191530(arg0, arg1);
                func_00102948(&D_008105D0, arg0 + 0x10);
                break;
            }
        }

        func_0018C6A0(arg1 + 0xB0, arg0 + 0x20, 0.20000000298023224f);
        func_0018C4B0(arg0 + 0x20, *(float *)(arg1 + 0xB4) + *(float *)(arg0 + 0x8C), 0.20000000298023224f);
        {
            char *e0 = (char *)&D_008105E0;
            func_0018C6A0(arg0 + 0x20, e0, 0.20000000298023224f);
            func_0018C4B0(e0, *(float *)(arg0 + 0x24), 0.20000000298023224f);
        }
skip:
        *(float *)(arg0 + 0x44) = func_001B1240(&D_008105D0, D_008105E0, D_008105E8);
        break;
    }
}

// NEARMISS func_00198F10  (vram 0x00198F10, 0x308 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 91.95% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// dead-instruction-padding + fp-register-coloring + call-arg-schedule: body/structure fully recovered (2-state animation/homing update: case 0 builds a homing-orientation matrix and blend target depending on arg1+0xD kind (2/1/other pick different Z offsets), advances an anim timer, caches D_008105...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Homing/anim update keyed on arg0+1 (sub-state), falling through 0->1:
// case 0: seeds an identity-ish transform D_70003400 (func_001029C0/func_00102C58
// against arg1+0xC0), then depending on arg1+0xD (kind 2 / 1 / else) picks a
// blend-target source (arg1+0xB0 or +0xA0, with an extra -14.0f nudge to
// arg0+0x24 for kind==1) and a matching Z constant for scratch quad D_70003600
// (-35.0/35.0/35.0), composes arg0+0x10 via func_001026A0 + func_001028B8,
// bumps arg0+0x14 by 6.0, advances the sub-state, clears arg0+2, and snapshots
// D_008105E0=arg0+0x20 / D_008105D0=arg0+0x10.
// case 1: if arg1+0x230 != 0x12, resets arg0's state (+6,+1) to 0 and skips to
// the tail. Else, if the global lock pair D_00810700==8 && D_00810701==3 and
// the target point (arg1+0xA0/+0xA8 minus a fixed 123.5/156.4 offset) is within
// an 8.0-unit box, snaps arg0+0x10/0x18/0x14 to a hardcoded homing pose
// (0x4314999A/0x43013333/282.4f), calls func_00191530, refreshes
// D_008105D0=arg0+0x10, and returns early. Otherwise blends two falloff curves
// (func_0018C6A0 then func_0018C4B0, factor 0.2) into arg0+0x20 and into
// D_008105E0. Tail: computes arg0+0x44 = func_001B1240(D_008105D0, D_008105E0,
// D_008105E8).

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

        func_001026A0(arg0 + 0x10, D_70003400, (void *)0x70003600);
        func_001028B8(arg0 + 0x10, arg0 + 0x20, arg0 + 0x10);
        *(float *)(arg0 + 0x14) = *(float *)(arg0 + 0x14) + 6.0f;
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
                return;
            }
        }

        func_0018C6A0(arg1 + 0xB0, arg0 + 0x20, 0.20000000298023224f);
        func_0018C4B0(arg0 + 0x20, *(float *)(arg1 + 0xB4) + *(float *)(arg0 + 0x8C), 0.20000000298023224f);
        func_0018C6A0(arg0 + 0x20, (char *)&D_008105E0, 0.20000000298023224f);
        func_0018C4B0((char *)&D_008105E0, *(float *)(arg0 + 0x24), 0.20000000298023224f);
skip:
        *(float *)(arg0 + 0x44) = func_001B1240(&D_008105D0, D_008105E0, D_008105E8);
        return;
    }
}

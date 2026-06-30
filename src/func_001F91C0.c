// NEARMISS func_001F91C0  (vram 0x001F91C0, 0x494 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 69.62% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Stack-frame layout + callee-saved regalloc permutation. mwcc hoists several &v[N] field-address pointers out of the loop into extra saved regs (s7/s8), growing the frame to 0xF0 vs target 0xD0 and shifting the v buffer to sp+0xB0 vs sp+0x90, so all sp-relative offsets differ; f20/f21/f22 FP color...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (233: 69.6%, 991202: 61.5%). Logic fully recovered. Dispatches on the
// entity kind byte p[3] (9/10/11/2) to pick a config table (D_0025DC00/DC90/
// DCF0/DB20) and a per-kind distance constant (100/500/100/40); the default
// path stages p+0xB0 from p[0x114] and calls func_001B5360. For each -1-
// terminated table row it resolves a target entity e = p[row[0]*4 + 0x110],
// builds a 0.4-scaled position block (sp+0x90, 16-float frame buffer 'v'),
// transforms it via func_001026A0/func_00102948, then per kind fills an aim/
// spread block (v[4..11]) with kind-specific pitch (-60/10/35/220) and 1.3x or
// 1.5x lateral scale f21/f22 from row[1]/row[2], gating on a func_0019A570 LOS
// test + a >10 range check for kind 10. For kind 2 it scales the spread by
// func_0011E748 of the entity's planar speed^2 (vx^2+vz^2, the mula/madd MAC),
// clamping f21/f22 toward each other. When active it builds a facing block
// (v[12..15] via func_001026A0/func_001028D0/func_00102760) and fires
// func_001F8D30(pos, aim, mid, facing, f22, f21, dist, D_0025DAE0).
//
// WALL: stack-frame layout + callee-saved register-allocation permutation. mwcc
// hoists several &v[N] field-address pointers out of the loop into extra saved
// registers (s7/s8), growing the frame to 0xF0 vs the target's 0xD0 and placing
// the v buffer at sp+0xB0 vs sp+0x90, so every sp-relative store offset differs.
// The f20/f21/f22 callee-saved FP coloring and s0/s1 base coloring also differ.
// Logic, the mula/madd MAC, both switch shapes and all calls match. This is the
// documented mwcc field-pointer-hoist / regalloc wall — permuter territory, not
// the clean-store nop and not jr-table/VU0.
extern void func_001026A0(void *a, void *b, void *c);
extern void func_00102760(void *a, void *b);
extern void func_001028D0(void *a, void *b, void *c);
extern void func_00102948(void *a, void *b);
extern float func_0011E748(float x);
extern int func_0019A570(void *a, void *b, int n, int m);
extern void func_001B5360(void *a);
extern void func_001F8D30(void *a, void *b, void *c, void *d, float e, float f, float g, void *h);

extern char D_0025DAE0[];
extern char D_0025DB20[];
extern char D_0025DC00[];
extern char D_0025DC90[];
extern char D_0025DCF0[];

void func_001F91C0(char *p) {
    float v[16];
    char *e;
    char *row;
    float scl;
    float f21, f22;
    int active;

    switch (*(unsigned char *)(p + 3)) {
    case 9:
        row = D_0025DC00;
        scl = 100.0f;
        break;
    case 10:
        row = D_0025DC90;
        scl = 500.0f;
        break;
    case 11:
        row = D_0025DCF0;
        scl = 100.0f;
        break;
    case 2:
        row = D_0025DB20;
        scl = 40.0f;
        break;
    default:
        func_00102948(p + 0xB0, *(char **)(p + 0x114) + 0xC0);
        func_001B5360(p);
        return;
    }

    while (*(int *)row != -1) {
        e = *(char **)(p + *(int *)row * 4 + 0x110);
        v[0] = 0.4f * *(float *)(e + 0);
        v[1] = 0.4f * *(float *)(e + 4);
        v[2] = 0.4f * *(float *)(e + 8);
        v[3] = 1.0f;
        func_001026A0(&v[0], e + 0x90, &v[0]);
        func_00102948(p + 0xB0, &v[0]);
        v[5] += 1.0f;
        v[9] = v[1] - 500.0f;
        switch (*(unsigned char *)(p + 3)) {
        case 2:
            f21 = 1.3f * *(float *)(row + 4);
            v[4] = v[0];
            v[5] = 220.0f;
            f22 = 1.3f * *(float *)(row + 8);
            v[6] = v[2];
            v[7] = 1.0f;
            v[8] = 0.0f;
            v[9] = 1.0f;
            v[10] = 0.0f;
            v[11] = 1.0f;
            active = 1;
            break;
        case 11:
            f21 = 1.3f * *(float *)(row + 4);
            v[4] = v[0];
            v[5] = 35.0f;
            f22 = 1.3f * *(float *)(row + 8);
            v[6] = v[2];
            v[7] = 1.0f;
            v[8] = 0.0f;
            v[9] = 1.0f;
            v[10] = 0.0f;
            v[11] = 1.0f;
            active = 1;
            break;
        case 10:
            if (func_0019A570(&v[0], &v[8], 6, 0)) {
                active = 0;
            } else if (*(float *)(e + 0xC4) <= 10.0f) {
                active = 0;
            } else {
                f21 = 1.5f * *(float *)(row + 4);
                v[4] = v[0];
                v[5] = 10.0f;
                f22 = 1.5f * *(float *)(row + 8);
                v[6] = v[2];
                v[7] = 1.0f;
                v[8] = 0.0f;
                v[9] = 1.0f;
                v[10] = 0.0f;
                v[11] = 1.0f;
                active = 1;
            }
            break;
        case 9:
            f21 = 1.3f * *(float *)(row + 4);
            v[4] = v[0];
            v[5] = -60.0f;
            f22 = 1.3f * *(float *)(row + 8);
            v[6] = v[2];
            v[7] = 1.0f;
            v[8] = 0.0f;
            v[9] = 1.0f;
            v[10] = 0.0f;
            v[11] = 1.0f;
            active = 1;
            break;
        default:
            return;
        }

        if (active) {
            if (*(unsigned char *)(p + 3) == 2) {
                float d = func_0011E748(*(float *)(e + 0x90) * *(float *)(e + 0x90)
                                        + *(float *)(e + 0x98) * *(float *)(e + 0x98));
                if (f21 <= f22) {
                    f22 *= d;
                    if (f22 < f21) {
                        f22 = f21;
                    }
                } else {
                    f21 *= d;
                    if (f21 < f22) {
                        f21 = f22;
                    }
                }
            }
            v[12] = *(float *)(e + 0);
            v[13] = *(float *)(e + 4);
            v[14] = *(float *)(e + 8);
            v[15] = 1.0f;
            func_001026A0(&v[12], e + 0x90, &v[12]);
            func_001028D0(&v[12], &v[12], e + 0xC0);
            func_00102760(&v[12], &v[12]);
            func_001F8D30(&v[0], &v[4], &v[8], &v[12], f22, f21, scl, D_0025DAE0);
        }
        row += 0x10;
    }
}

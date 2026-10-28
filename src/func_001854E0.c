// NEARMISS func_001854E0  (vram 0x001854E0, 0x280 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.47% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful.
// Matching wave 2026-10-27 (was 88.66%): the staging scale is 260.0f (0x43820000; the old C said
// 65.0f), the scratchpad vectors are float[4] arrays, the distance is stored to a volatile
// scratchpad word and re-read for the 20.0f offset, and the draw call uses the matched callers'
// prototype (rgba as the fifth integer argument) with width/height from one chained 3.0f.
// Remaining differences: register colouring and scheduling of the packed-colour argument at
// the final draw call (the original keeps $a0 free and loads 3.0f into $f13 first).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// AI candidate-target validation / lock-on scoring, sibling of func_00185E30.
// Stages arg0's transform: scales arg0+0xC0 by 260.0f into D_700038A0 via
// func_00103230, composes with arg0+0xA0 via func_001028B8, sets
// D_700038A0[3] = 1.0f, copies arg0+0xA0 into D_700038B0 via func_00102948.
// If func_0019A570(D_700038B0, D_700038A0, 7, 0x20) succeeds: if the shared
// target pointer at 0x700031D4 is set and func_00183AC0() accepts it, marks
// its flag byte +0xA = 0x80. Runs func_001031E0(D_700038A0, D_700031B0),
// sets arg0+0xBC=1.0f, arg0's sub-object (+0x1F0)+0x20=1, copies D_700038A0
// into sub-object+0x10 via func_00102948, then func_001028D0 computes a delta
// into D_700038D0. Its length (func_0011E748 of the sum of squares) is stored
// at 0x70003A24; if >= 260.0f (or the first test failed), sub-object+0x24 =
// 0.0f. Else the stored distance is reduced by 20.0f: if negative, +0x24 =
// 1.0f; else +0x24 = (240.0f - dist) / 240.0f (falloff ramp). Finally writes
// an RGBA word set (red from func_00122BB8()'s bits, alpha 0x80) at
// 0x700038B0..BC and issues a func_001CD520 sprite draw with it packed.

extern void func_00102948(void *dst, void *src);
extern void func_001028B8(void *a, void *b, void *c);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_00103230(void *a, void *b, float c);
extern void func_001031E0(float *a, float *b);
extern int func_0019A570(float *a, float *b, int c, int d);
extern float func_0011E748(float x);
extern int func_00122BB8(void);
extern int func_00183AC0(unsigned char *a);
extern int func_001CD520(int, int, void *, long long, int, float, float, float);

extern float D_700031B0[4];
extern float D_700038A0[4];
extern float D_700038B0[4];
extern float D_700038D0[4];

void func_001854E0(char *arg0) {
    char *sub = arg0 + 0x1F0;
    unsigned char *target;

    func_00103230(D_700038A0, arg0 + 0xC0, 260.0f);
    func_001028B8(D_700038A0, D_700038A0, arg0 + 0xA0);
    D_700038A0[3] = 1.0f;
    func_00102948(D_700038B0, arg0 + 0xA0);

    if (func_0019A570(D_700038B0, D_700038A0, 7, 0x20) != 0) {
        target = *(unsigned char **)0x700031D4;
        if (target != 0 && func_00183AC0(target) != 0) {
            target[0xA] = 0x80;
        }
        func_001031E0(D_700038A0, D_700031B0);
        *(float *)(arg0 + 0xBC) = 1.0f;
        *(int *)(sub + 0x20) = 1;
        func_00102948(sub + 0x10, D_700038A0);
        func_001028D0(D_700038D0, D_700038A0, D_700038B0);
        {
            float dz, dy, dx;
            float d;
            dx = *(float *)0x700038D0;
            dy = *(float *)0x700038D4;
            dz = *(float *)0x700038D8;
            d = func_0011E748((dx * dx) + (dy * dy) + (dz * dz));
            *(volatile float *)0x70003A24 = d;
            if (!(d < 260.0f)) {
                *(float *)(sub + 0x24) = 0.0f;
            } else {
                float d2 = *(volatile float *)0x70003A24 - 20.0f;
                *(volatile float *)0x70003A24 = d2;
                if (d2 < 0.0f) {
                    *(float *)(sub + 0x24) = 1.0f;
                } else {
                    *(float *)(sub + 0x24) = (240.0f - d2) / 240.0f;
                }
            }
        }
    } else {
        *(float *)(sub + 0x24) = 0.0f;
    }

    *(int *)0x700038B0 = ((func_00122BB8() >> 0xF) & 0x1F) + 0x40;
    *(int *)0x700038B4 = 0;
    *(int *)0x700038B8 = 0;
    *(int *)0x700038BC = 0x80;

    {
        float w;
        float h;
        h = w = 3.0f;
        func_001CD520(0, 2, D_700038A0, 0x20045BA5154222DCULL,
                      *(int *)0x700038B0 | ((*(int *)0x700038B4 << 8) | ((*(int *)0x700038BC << 24) | (*(int *)0x700038B8 << 16))),
                      w, h, 2.0f);
    }
}

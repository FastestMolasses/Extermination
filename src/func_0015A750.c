// NEARMISS func_0015A750  (vram 0x0015A750, 0x3A0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 73.04% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// 73.04 (mwcc233 -O4,p -sdatathreshold 0; 232 instrs). Byte-identical: the whole prologue/setup, the jr-table dispatch INCLUDING the jtbl_0026D390 reloc, all 7 case bodies (D_00248280 + count), the loop guard, the loop preheader (`mul.s fs1,fv0,fs0`), and the entire epilogue/phase-wrap tail. The re...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS: per-frame particle/spark emitter for one actor `p`.
//   Setup:
//     - D_700036A0 (a 16-byte scratchpad transform) is initialised by
//       func_001029C0 and then concatenated with the actor's world matrix
//       at p+0xB0 by func_00102918.
//     - the scratchpad float at 0x700036D4 (== D_700036A0+0x34, the matrix
//       Y translation) is advanced by +2.0f each frame.
//     - `handle` = func_001CCF70(p+0xB0): the emitter/effect handle for this
//       actor's position.
//     - `rng`    = p->0x1F4, a linear-congruential state (x = x*37 + 11).
//     - `dt`     = p->0x20 / 60.0f (frame delta in seconds).
//   The actor kind (short at p+0x54, 0..6) selects, through jtbl_0026D390,
//   both the global sprite/blend mode D_00248280 (0x18 or 0x20) and the
//   number of particles `count` to spawn (2..5).  Out-of-range kinds fall
//   straight through with `count` unset (the original relies on the
//   uninitialised register, which is why the guard below reads it directly).
//   For particles i = 1 .. count-1:
//     a     = p->0x1F0 + 0.5f * (rand01() + 0.0001f), wrapped into (.., 2.0f]
//             by subtracting 1.0f -- this is the emitter's hue/phase.
//     ratio = (i + 1) / count -- the particle's share of the emitter size.
//     D_00248210 / D_00248218 = the kind's base X/Z size
//             (D_00248120[kind*5 + 0] and [kind*5 + 2]) scaled by `ratio`
//             and then shrunk by 5.0f.
//     D_0024822C = 64.0f * dt   (lifetime/scroll rate)
//     D_0024823C = 0            (flags)
//     func_001CFAE0 builds the particle descriptor into the 0x60-byte stack
//     buffer `work` from (mode 0, the scratchpad matrix, a, b = rand01(),
//     dt, 0.1f); func_001CFBE0 submits one instance of it to `handle` using
//     the shared template D_00248200.
//   Finally the emitter phase p->0x1F0 advances by 0.01f per frame and wraps
//   the same way (>2.0f -> -1.0f).

extern void func_001029C0(char *m);
extern void func_00102918(char *dst, char *m, char *v);
extern int func_001CCF70(char *m);
extern void func_001CFAE0(char *out, int mode, char *m, float hue, float rnd,
                          float dt, float k);
extern void func_001CFBE0(int handle, int n, char *desc, char *out);

extern float D_00248120[];
extern char D_00248200[];
extern float D_00248210;
extern float D_00248218;
extern float D_0024822C;
extern int D_0024823C;
extern int D_00248280;
extern char D_700036A0[];

void func_0015A750(char *p)
{
    char work[0x60];
    int i;
    int count;
    int rng;
    int handle;
    float dt;

    func_001029C0(D_700036A0);
    func_00102918(D_700036A0, D_700036A0, p + 0xB0);
    *(float *)0x700036D4 = *(float *)0x700036D4 + 2.0f;
    handle = func_001CCF70(p + 0xB0);
    rng = *(int *)(p + 0x1F4);
    dt = *(float *)(p + 0x20) / 60.0f;

    switch (*(short *)(p + 0x54)) {
    case 0:
        D_00248280 = 0x18;
        count = 2;
        break;
    case 1:
        D_00248280 = 0x20;
        count = 3;
        break;
    case 2:
        D_00248280 = 0x20;
        count = 5;
        break;
    case 3:
        D_00248280 = 0x18;
        count = 3;
        break;
    case 4:
        D_00248280 = 0x20;
        count = 5;
        break;
    case 5:
        D_00248280 = 0x18;
        count = 3;
        break;
    case 6:
        D_00248280 = 0x20;
        count = 4;
        break;
    }

    for (i = 1; i < count; i++) {
        float a;
        float b;
        float ratio;

        a = *(float *)(p + 0x1F0)
            + 0.5f * ((float)((rng >> 16) & 0xFFFF) / 65535.0f + 0.0001f);
        rng = rng * 37 + 11;
        if (!(a <= 2.0f)) {
            a = a - 1.0f;
        }
        ratio = (float)(i + 1) / (float)count;
        D_00248210 = D_00248120[*(short *)(p + 0x54) * 5] * ratio;
        b = (float)((rng >> 16) & 0xFFFF) / 65535.0f + 0.0001f;
        rng = rng * 37 + 11;
        D_00248218 = D_00248120[*(short *)(p + 0x54) * 5 + 2] * ratio;
        D_00248210 = D_00248210 - 5.0f;
        D_00248218 = D_00248218 - 5.0f;
        D_0024822C = 64.0f * dt;
        D_0024823C = 0;
        func_001CFAE0(work, 0, D_700036A0, a, b, dt, 0.1f);
        func_001CFBE0(handle, 1, D_00248200, work);
    }

    *(float *)(p + 0x1F0) = *(float *)(p + 0x1F0) + 0.01f;
    if (!(*(float *)(p + 0x1F0) <= 2.0f)) {
        *(float *)(p + 0x1F0) = *(float *)(p + 0x1F0) - 1.0f;
    }
}

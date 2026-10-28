// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x700038A0 0x700038B0

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 70.92% NEARMISS).
// Fixes read from the original instructions (twin: func_001E5AC0):
//  - the area selection is one switch (case 0xF01, then case 0x1500 with the
//    nested AREA21 camera switch: 0x1F returns, 0x1E -> 3, 0x20 -> 9);
//  - the default count 0x14 is stored before the area id is built;
//  - the wrapped camera cell lives in the scratchpad vector 0x700038B0 (a
//    relocated extern in the tile loop), and the row tilt in 0x70003A20,
//    built as base then a compound += / -= of the wobble (reverse = -base);
//  - both direction arms ADD the drift step to the +0x18 phase;
//  - a write-only tile counter, the UV jitter built in steps
//    (convert, /= 65535, += 0.0001), func_001CFFE0's fifth argument 1;
//  - the first fog call spells its shared zero as `(w = 0.0f)` with w = 0.
//
// Emit 6 rows x 6 depth steps x 3 vertical steps of weather particles.
// The area entry selects the particle count (D_002551F0); AREA21's scripted
// camera cases can change the count or suppress this renderer. Flags bit 1
// negates the row tilt and reverses the phase direction. 0021B9A0 configures
// fog, not audio: disable fog, select near=0/far=300, then restore the
// default fog mode.
//
// Camera position wraps each grid axis around its local 200-unit cell. Each
// tile's transform uses the SDK X-rotation helper and camera-relative
// translation. Row tables supply size/color; the local seed advances by
// seed*37+11 per tile. 001CFAE0 receives phase, random fraction+0.0001,
// color multiplier 1, and fade interval 0.000001 (f12..f15); it packs these
// for VU59 as phase/color/fade/seed. Phase and drift advance once per row.
//
// Native recovery and original DMA evidence: extermination-port/docs/SNOW_PARTICLES.md.
extern int float_to_int(float f);
extern void func_001026A0(void *dst, void *src, void *xf);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_00102900(void *dst, void *src, float s);
extern void func_00102948(void *dst, void *src);
extern void func_001029C0(void *m);
extern void func_00102B08(void *a, void *b, float ang);
extern float func_0011E2A8(float a);
extern void func_001CFAE0(void *out, int n, void *p, float a, float b, float c, float d);
extern void func_001CFFE0(int a, int b, void *src, void *out, int e);
extern void func_0021B9A0(int id, float a, float b);

extern int D_00255170;
extern int D_00255190;
extern int D_002551A0;
extern int D_002551B0;
extern int D_002551C0;
extern int D_002551F0;
extern char D_00255200[];
extern unsigned char D_008101E4;
extern short D_0081024E;
extern float D_008105D0;
extern float D_008105D4;
extern float D_008105D8;
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern int D_700036A0;
extern int D_700036D0;
extern float D_700038A0[4];
extern float D_700038B0[4];

void func_001E67C0(void *weather, int flags, int initial_seed, float strength) {
    int count;      /* tiles emitted; counted but never read */
    int i;          /* row (x step) */
    int j;          /* depth (z step) */
    int k;          /* vertical (y step) */
    char *row;      /* row table entry, 0x30 bytes */
    char *e;        /* per-row phase state: [0] phase, [+0x18] drift */
    int seed;
    int mode;
    int area;
    int n;
    float fx, fy, fz;
    float s13, st, y, uv;
    int sp90[24];

    seed = initial_seed;
    e = (char *)weather + 0x1F0;
    D_002551F0 = 0x14;
    area = (D_00810700 << 8) + D_00810701;
    count = 0;
    switch (area) {
    case 0xF01:
        D_002551F0 = 0xA;
        break;
    case 0x1500:
        D_002551F0 = 6;
        if (D_008101E4 == 3) {
            switch (D_0081024E) {
            case 0x1F:
                return;
            case 0x1E:
                D_002551F0 = 3;
                break;
            case 0x20:
                D_002551F0 = 9;
                break;
            }
        }
        break;
    }
    {
        float w = 0.0f;
        func_0021B9A0(2, w, (w = 0.0f));
    }
    func_0021B9A0(3, 0.0f, 300.0f);
    n = float_to_int(fx = D_008105D0);
    fy = D_008105D4;
    *(float *)0x700038B0 = (float)((n + 0x186A0) % 200);
    n = float_to_int(fy);
    fz = D_008105D8;
    *(float *)0x700038B4 = (float)((n + 0x186A0) % 200);
    n = float_to_int(fz);
    *(float *)0x700038B8 = (float)((n + 0x186A0) % 200);
    *(float *)0x700038B0 += (fx - (float)float_to_int(fx));
    *(float *)0x700038B4 += (fy - (float)float_to_int(fy));
    *(float *)0x700038B8 += (fz - (float)float_to_int(fz));
    mode = flags & 2;
    row = D_00255200;
    s13 = 1.3f * strength;
    st = 0.004f * strength;
    i = 0;
    do {
        if (mode != 0) {
            *(float *)0x70003A20 = -*(float *)row;
            *(float *)0x70003A20 -= 0.5f * (*(float *)row * func_0011E2A8(6.2831855f * *(float *)(e + 0x18)));
        } else {
            *(float *)0x70003A20 = *(float *)row;
            *(float *)0x70003A20 += 0.5f * (*(float *)row * func_0011E2A8(6.2831855f * *(float *)(e + 0x18)));
        }
        for (j = 0; j < 6; j++) {
            for (k = 0; k < 3; k++) {
                D_700038A0[0] = 200.0f * ((float)i / 6.0f) - D_700038B0[0];
                D_700038A0[2] = 200.0f * ((float)j / 6.0f) - D_700038B0[2];
                D_700038A0[1] = 100.0f * ((float)k / 3.0f) - D_700038B0[1];
                D_700038A0[3] = 1.0f;
                while (D_700038A0[0] < 0.0f) {
                    D_700038A0[0] += 200.0f;
                }
                while (D_700038A0[2] < 0.0f) {
                    D_700038A0[2] += 200.0f;
                }
                while (D_700038A0[1] < 0.0f) {
                    D_700038A0[1] += 100.0f;
                }
                D_700038A0[0] -= 100.0f;
                D_700038A0[2] -= 200.0f;
                D_700038A0[1] -= 50.0f;
                func_001029C0(&D_700036A0);
                func_00102B08(&D_700036A0, &D_700036A0, (3.1415927f * *(float *)0x70003A20) / 180.0f);
                func_001026A0(&D_700036D0, &D_700036A0, D_700038A0);
                func_001028B8(&D_700036D0, &D_700036D0, &D_008105D0);
                *(float *)0x700036DC = 1.0f;
                func_00102900(&D_002551B0, row + 0x10, 1.0f);
                func_00102900(&D_00255190, row + 0x20, s13);
                func_00102948(&D_002551C0, &D_002551B0);
                func_00102948(&D_002551A0, &D_00255190);
                uv = (float)((seed >> 0x10) & 0xFFFF);
                uv /= 65535.0f;
                uv += 0.0001f;
                seed = (seed * 0x25) + 0xB;
                func_001CFAE0(sp90, 0, &D_700036A0, *(float *)e, uv, 1.0f, 0.000001f);
                func_001CFFE0(3, 3, &D_00255170, sp90, 1);
                count++;
            }
        }
        if (mode != 0) {
            *(float *)(e + 0x18) += st;
            y = *(float *)e - 1.5f * (*(float *)(row + 8) * strength);
            *(float *)e = y;
            if (y < 1.0f) {
                *(float *)e += 1.0f;
            }
        } else {
            *(float *)(e + 0x18) += st;
            y = *(float *)e + 1.5f * (*(float *)(row + 8) * strength);
            *(float *)e = y;
            if (!(y <= 2.0f)) {
                *(float *)e -= 1.0f;
            }
        }
        i += 1;
        e += 4;
        row += 0x30;
    } while (i < 6);
    func_0021B9A0(1, 0.0f, 0.0f);
}

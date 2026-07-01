// NEARMISS func_001E67C0  (vram 0x001E67C0, 0x7A0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 71.55% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Frame-size/register-pressure mismatch: this build colors one extra callee-saved integer register (s8) and one extra callee-saved float register versus the target (target uses 8 int + 4 float saved regs), attributable to a larger simultaneous live-range set among the loop temporaries (mode/mirror/...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Explosion/impact VFX spawner. arg0=owner entity, arg1=flags (bit1 selects a
// mirrored X offset), arg2=seed (advanced per-particle with an LCG), fparg0=
// scale. Picks a sound-set id (D_002551F0) from the room-transition byte pair
// (D_00810700:D_00810701) and, in a couple of special cases, from the weapon
// sub-mode (D_008101E4==3) and camera-shot id (D_0081024E); plays two impact
// cues via func_0021B9A0. Builds a jittered floor-snap origin (D_008105D0..D8
// each wrapped into a 200-unit cell + fractional part). For 6 height rows x 6
// x-steps x 3 z-steps, computes a grid offset (mirrored in X when arg1&2),
// wraps each axis into its cell range, builds a rotation/translation matrix
// (func_001029C0/func_00102B08/func_001026A0/func_001028B8) from
// D_008105D0..D8, blends two secondary transforms (func_00102900/
// func_00102948) from the per-row table at var_s4, and spawns a particle
// (func_001CFAE0/func_001CFFE0) with an LCG-seeded UV jitter. Every 6 rows it
// advances the emitter's z-oscillation phase (var_s5+0x18) by 0.004*fparg0 and
// bounces the per-row radius (var_s5+0) between 1.0 and 2.0. Finally stops the
// sound loop via func_0021B9A0(1,0,0).
extern int float_to_int(float f);
extern void func_001026A0(void *dst, void *src, void *xf);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_00102900(void *dst, void *src, float s);
extern void func_00102948(void *dst, void *src);
extern void func_001029C0(void *m);
extern void func_00102B08(void *a, void *b, float ang);
extern float func_0011E2A8(float a);
extern void func_001CFAE0(void *out, int n, void *p, float a, float b, float c, float d);
extern void func_001CFFE0(int a, int b, void *src, void *out);
extern void func_0021B9A0(int id, float a, float b);

extern int D_00255170;
extern int D_00255190;
extern int D_002551A0;
extern int D_002551B0;
extern int D_002551C0;
extern int D_002551F0;
extern int D_00255200;
extern unsigned char D_008101E4;
extern short D_0081024E;
extern float D_008105D0;
extern float D_008105D4;
extern float D_008105D8;
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern int D_700036A0;
extern int D_700036D0;
extern int D_700038A0;

void func_001E67C0(void *arg0, int arg1, int arg2, float fparg0) {
    unsigned char *e;
    int seed;
    int mode;
    float cellX, cellY, cellZ;
    int mirror;
    void *row;
    int i, xIdx, zIdx;
    float x, y, z, ang;
    float uv;
    unsigned char sp[0x60];

    e = (unsigned char *)arg0 + 0x1F0;
    seed = arg2;
    mode = (D_00810700 << 8) + D_00810701;
    D_002551F0 = 0x14;
    if (mode != 0x1500) {
        if (mode != 0xF01) {
            /* keep default D_002551F0 = 0x14 */
        } else {
            D_002551F0 = 0xA;
        }
    } else {
        D_002551F0 = 6;
        if (D_008101E4 == 3) {
            switch (D_0081024E) {
            case 0x1E:
                D_002551F0 = 3;
                break;
            case 0x1F:
                return;
            case 0x20:
                D_002551F0 = 9;
                break;
            }
        }
    }
    func_0021B9A0(2, 0.0f, 0.0f);
    func_0021B9A0(3, 0.0f, 300.0f);

    cellX = (float)((float_to_int(D_008105D0) + 0x186A0) % 200);
    cellY = (float)((float_to_int(D_008105D4) + 0x186A0) % 200);
    cellZ = (float)((float_to_int(D_008105D8) + 0x186A0) % 200);
    cellX = cellX + (D_008105D0 - (float)float_to_int(D_008105D0));
    cellY = cellY + (D_008105D4 - (float)float_to_int(D_008105D4));
    cellZ = cellZ + (D_008105D8 - (float)float_to_int(D_008105D8));

    mirror = arg1 & 2;
    row = &D_00255200;
    i = 0;
    ang = 0.004f * fparg0;
    do {
        if (mirror != 0) {
            x = -(*(float *)row) - 0.5f * (*(float *)row * func_0011E2A8(6.2831855f * *(float *)(e + 0x18)));
        } else {
            x = *(float *)row + 0.5f * (*(float *)row * func_0011E2A8(6.2831855f * *(float *)(e + 0x18)));
        }
        for (zIdx = 0; zIdx < 6; zIdx++) {
            for (xIdx = 0; xIdx < 3; xIdx++) {
                *(float *)0x700038A0 = 200.0f * ((float)i / 6.0f) - cellX;
                *(float *)0x700038A8 = 200.0f * ((float)zIdx / 6.0f) - cellZ;
                *(float *)0x700038A4 = 100.0f * ((float)xIdx / 3.0f) - cellY;
                *(int *)0x700038AC = 0x3F800000;
                while (*(float *)0x700038A0 < 0.0f) {
                    *(float *)0x700038A0 = *(float *)0x700038A0 + 200.0f;
                }
                while (*(float *)0x700038A8 < 0.0f) {
                    *(float *)0x700038A8 = *(float *)0x700038A8 + 200.0f;
                }
                while (*(float *)0x700038A4 < 0.0f) {
                    *(float *)0x700038A4 = *(float *)0x700038A4 + 100.0f;
                }
                *(float *)0x700038A0 = *(float *)0x700038A0 - 100.0f;
                *(float *)0x700038A8 = *(float *)0x700038A8 - 200.0f;
                *(float *)0x700038A4 = *(float *)0x700038A4 - 50.0f;
                func_001029C0(&D_700036A0);
                func_00102B08(&D_700036A0, &D_700036A0, (3.1415927f * x) / 180.0f);
                func_001026A0(&D_700036D0, &D_700036A0, &D_700038A0);
                func_001028B8(&D_700036D0, &D_700036D0, &D_008105D0);
                *(int *)0x700036DC = 0x3F800000;
                func_00102900(&D_002551B0, (char *)row + 0x10, 1.0f);
                func_00102900(&D_00255190, (char *)row + 0x20, 1.3f * fparg0);
                func_00102948(&D_002551C0, &D_002551B0);
                func_00102948(&D_002551A0, &D_00255190);
                uv = (float)((seed >> 0x10) & 0xFFFF) / 65535.0f;
                seed = (seed * 0x25) + 0xB;
                func_001CFAE0(sp, 0, &D_700036A0, 1.0f, *(float *)(e + 0), uv + 0.0001f, 1.0f);
                func_001CFFE0(3, 3, &D_00255170, sp);
            }
        }
        if (mirror != 0) {
            *(float *)(e + 0x18) = *(float *)(e + 0x18) + ang;
            y = *(float *)e - 1.5f * (*(float *)((char *)row + 8) * fparg0);
            *(float *)e = y;
            if (y < 1.0f) {
                z = *(float *)e + 1.0f;
                *(float *)e = z;
            }
        } else {
            *(float *)(e + 0x18) = *(float *)(e + 0x18) + ang;
            y = *(float *)e + 1.5f * (*(float *)((char *)row + 8) * fparg0);
            *(float *)e = y;
            if (!(y <= 2.0f)) {
                z = *(float *)e - 1.0f;
                *(float *)e = z;
            }
        }
        i += 1;
        e += 4;
        row = (char *)row + 0x30;
    } while (i < 6);
    func_0021B9A0(1, 0.0f, 0.0f);
}

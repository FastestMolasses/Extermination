// NEARMISS func_001E67C0 (vram 0x001E67C0, 0x7A0 bytes).
// Corrected readable weather tile emitter; original assembly remains linked.
// Measured 70.915985% (1784-byte C versus 1952-byte original), mwcc2.3.3.
// The prior incorrect arguments scored 71.55123%; similarity is not fidelity.
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Emit 6 rows x 6 depth steps x 3 vertical steps of weather particles.
// The area entry selects descriptor particle count; AREA21's scripted
// camera cases can change the count or suppress this renderer. Flags bit 1
// reverses the row tilt and phase direction. 0021B9A0 configures fog, not
// audio: disable fog, select near=0/far=300, then restore the default fog mode.
//
// Camera position wraps each grid axis around its local cell. Each tile's
// transform uses the original SDK X-rotation helper and camera-relative
// translation. Row tables supply size/color; the local seed advances by
// seed*37+11 per tile. 001CFAE0 receives phase, random fraction+0.0001,
// color multiplier 1, and fade interval 0.000001; it packs these for VU59 as
// phase/color/fade/seed. Both phase and drift advance once per row.
//
// Native recovery and original DMA evidence: extermination-port/docs/SNOW_PARTICLES.md.
// Earlier source had all four float call arguments wrong and mislabeled
// this renderer as an explosion/sound spawner. Do not infer pixel fidelity
// or exact SDK arithmetic merely from a decompilation similarity score.
extern int float_to_int(float f);
extern void func_001026A0(void *dst, void *src, void *xf);
extern void func_001028B8(void *dst, void *a, void *b);
extern void func_00102900(void *dst, void *src, float s);
extern void func_00102948(void *dst, void *src);
extern void func_001029C0(void *m);
extern void func_00102B08(void *a, void *b, float drift_step);
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

void func_001E67C0(void *weather, int flags, int initial_seed, float strength) {
    unsigned char *phases;
    int seed;
    int area_entry;
    float cellX, cellY, cellZ;
    int reverse;
    void *row_data;
    int row_index, vertical_step, depth_step;
    float tilt, next_phase, wrapped_phase, drift_step;
    float seed_fraction;
    unsigned char draw_state[0x60];

    phases = (unsigned char *)weather + 0x1F0;
    seed = initial_seed;
    area_entry = (D_00810700 << 8) + D_00810701;
    D_002551F0 = 0x14;
    if (area_entry != 0x1500) {
        if (area_entry != 0xF01) {
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

    reverse = flags & 2;
    row_data = &D_00255200;
    row_index = 0;
    drift_step = 0.004f * strength;
    do {
        if (reverse != 0) {
            tilt = -(*(float *)row_data) - 0.5f * (*(float *)row_data * func_0011E2A8(6.2831855f * *(float *)(phases + 0x18)));
        } else {
            tilt = *(float *)row_data + 0.5f * (*(float *)row_data * func_0011E2A8(6.2831855f * *(float *)(phases + 0x18)));
        }
        for (depth_step = 0; depth_step < 6; depth_step++) {
            for (vertical_step = 0; vertical_step < 3; vertical_step++) {
                *(float *)0x700038A0 = 200.0f * ((float)row_index / 6.0f) - cellX;
                *(float *)0x700038A8 = 200.0f * ((float)depth_step / 6.0f) - cellZ;
                *(float *)0x700038A4 = 100.0f * ((float)vertical_step / 3.0f) - cellY;
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
                func_00102B08(&D_700036A0, &D_700036A0, (3.1415927f * tilt) / 180.0f);
                func_001026A0(&D_700036D0, &D_700036A0, &D_700038A0);
                func_001028B8(&D_700036D0, &D_700036D0, &D_008105D0);
                *(int *)0x700036DC = 0x3F800000;
                func_00102900(&D_002551B0, (char *)row_data + 0x10, 1.0f);
                func_00102900(&D_00255190, (char *)row_data + 0x20, 1.3f * strength);
                func_00102948(&D_002551C0, &D_002551B0);
                func_00102948(&D_002551A0, &D_00255190);
                seed_fraction = (float)((seed >> 0x10) & 0xFFFF) / 65535.0f;
                seed = (seed * 0x25) + 0xB;
                func_001CFAE0(draw_state, 0, &D_700036A0, *(float *)(phases + 0), seed_fraction + 0.0001f, 1.0f, 0.000001f);
                func_001CFFE0(3, 3, &D_00255170, draw_state);
            }
        }
        if (reverse != 0) {
            *(float *)(phases + 0x18) = *(float *)(phases + 0x18) + drift_step;
            next_phase = *(float *)phases - 1.5f * (*(float *)((char *)row_data + 8) * strength);
            *(float *)phases = next_phase;
            if (next_phase < 1.0f) {
                wrapped_phase = *(float *)phases + 1.0f;
                *(float *)phases = wrapped_phase;
            }
        } else {
            *(float *)(phases + 0x18) = *(float *)(phases + 0x18) + drift_step;
            next_phase = *(float *)phases + 1.5f * (*(float *)((char *)row_data + 8) * strength);
            *(float *)phases = next_phase;
            if (!(next_phase <= 2.0f)) {
                wrapped_phase = *(float *)phases - 1.0f;
                *(float *)phases = wrapped_phase;
            }
        }
        row_index += 1;
        phases += 4;
        row_data = (char *)row_data + 0x30;
    } while (row_index < 6);
    func_0021B9A0(1, 0.0f, 0.0f);
}

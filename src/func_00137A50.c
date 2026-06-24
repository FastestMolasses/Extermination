// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-frame update for an animated entity. arg0 (p) is the entity, arg1 (q) is
// a state/timer block. A 3-state machine on p[5]: state 0 advances to 1 and
// seeds an idle clip; state 1 waits for flag 0x1000 in q[0] then advances to 2
// and seeds the active clip; state 2 runs the steady behaviour (angle update
// via func_001B1240/func_001B12B0 with const pi/40, two randomized countdown
// timers driven by func_00122BB8, and a periodic func_0014D1E0 retrigger).
// Then writes p[0] (1 or 2 depending on q[0x40]), runs the standard per-frame
// chain (physics/anim/collision/etc.), advances anim time into q[0], and calls
// the per-entity vtable hook at p[0x4C]. anim_clip_init takes (p, idx, f, f) so
// the case index lands in $a1 (reusing the case-1 comparator); the float const
// must be the exact pi/40 bits (0x3DA0D97C); the 1-p[0xD] result is held in a
// temp so the test reuses it (& 0xFF) rather than reloading. mwcc 2.3.3 only;
// pinned 991202 reaches 95.0% (fills the clean-store delay-slot nop). objdiff
// 100.0 vs expected.
extern int anim_advance_time(char *p, float t);
extern void anim_clip_init(char *p, int idx, float b, float c);
extern int func_00122BB8(void);
extern void func_00131ED0(char *p);
extern void func_001381D0(char *p, char *q);
extern void func_0014D1E0(char *a, char *b, int n);
extern float func_001B1240(char *p, float b, float c);
extern float func_001B12B0(float a, float b, float c);
extern void func_001B17A0(char *p);
extern void func_001B4810(char *p);
extern void func_001B5360(char *p);
extern void func_001C68C0(char *p);
extern float D_00810360;
extern float D_00810368;

void func_00137A50(char *p, char *q) {
    int v;
    short h;
    signed char c;
    unsigned char b;

    switch (*(unsigned char *)(p + 5)) {
    case 0:
        *(unsigned char *)(p + 5) = *(unsigned char *)(p + 5) + 1;
        anim_clip_init(p, 0, 0.0f, 0.0f);
        break;
    case 1:
        if (*(int *)(q + 0x0) & 0x1000) {
            *(unsigned char *)(p + 5) = *(unsigned char *)(p + 5) + 1;
            anim_clip_init(p, 1, 0.0f, 0.0f);
        }
        break;
    case 2:
        *(float *)(q + 0x2C) = func_001B1240(p + 0xB0, D_00810360, D_00810368);
        *(float *)(p + 0xC4) = func_001B12B0(*(float *)(q + 0x2C), *(float *)(p + 0xC4), 0.07853981852531433f);
        v = *(int *)(q + 0x4);
        if (v != 0) {
            *(int *)(q + 0x4) = v - 1;
        } else {
            b = 1 - *(unsigned char *)(p + 0xD);
            *(unsigned char *)(p + 0xD) = b;
            if (b & 0xFF) {
                *(int *)(q + 0x4) = ((func_00122BB8() >> 0x12) & 0xFF) + 0xA;
            } else {
                *(int *)(q + 0x4) = ((func_00122BB8() >> 0x13) & 0x3F) + 0xA;
            }
        }
        h = *(short *)(q + 0x32);
        if (h != 0) {
            *(short *)(q + 0x32) = h - 1;
        } else {
            c = *(signed char *)(q + 0x43) + 1;
            *(signed char *)(q + 0x43) = c;
            if (c >= 2) {
                *(signed char *)(q + 0x43) = 0;
                *(short *)(q + 0x32) = ((func_00122BB8() >> 0xD) & 0x1FF) + 0x244;
            } else {
                *(short *)(q + 0x32) = ((func_00122BB8() >> 0x12) & 0x7F) + 0x1E;
            }
            func_0014D1E0(p + 0xB0, p + 0xC0, 2);
        }
        break;
    }

    if (*(signed char *)(q + 0x40) != 0) {
        *(char *)(p + 0x0) = 2;
    } else {
        *(char *)(p + 0x0) = 1;
    }

    func_001B4810(p);
    func_001B5360(p);
    func_001381D0(p, q);
    *(int *)(q + 0x0) = anim_advance_time(p, *(float *)(q + 0x28));
    func_00131ED0(p);
    func_001C68C0(p);
    func_001B17A0(p);
    (*(void (**)(char *))(p + 0x4C))(p);
}

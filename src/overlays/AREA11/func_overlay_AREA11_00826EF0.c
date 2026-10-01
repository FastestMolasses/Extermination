// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA11 overlay, runtime 0x00826F30 (splat/link name 00826EF0; overlay code is
// linked 0x40 below where it runs), 0x4D0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA11; lane A11C).
// Covers the splat pieces 00826EF0, 00826F30 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: gun probe at runtime 0x826F30 (same code as the AREA01 probe
// 0x8282F0, src/overlays/AREA01/func_overlay_AREA01_008282B0.c). Casts
// the (3, -2, 0) ray through the record B matrix. Result code 0 when
// func_0019AA80 found nothing and func_0019A570 fails (0x700031D0..D8 are
// then restored); 1 when func_0019A570 hits and the offset is longer than
// 10000 or the hit is kind 1 with byte +3 of its record in 0x10..0x13 (it
// draws the beam, func_001E2BA0); 2 otherwise (it records the first kind-1
// hit at +0x204 and draws the sprite, state 4, or the tinted beam, +0x200 >=
// 13). Returns 0, or for code 2: 2 when +0x204 equals 0x700031D4, else 1.
typedef struct { float x, y, z, w; } Vec4;
typedef struct {
    float f1F0, f1F4, f1F8, f1FC;
    int f200, f204;
} Work;
#define W ((Work *)(self + 0x1F0))
#define SPI(a) (*(int *)(a))
extern char *D_00275B40;
extern Vec4 D_overlay_AREA11_0082A730[1];
extern float D_700038A0[4];
extern float D_700038B0[4];
extern float D_700038C0[4];
extern float D_700038D0[4];
extern float D_700031B0[4];
extern int D_700031D0[4];
extern int D_700031D4[4];
extern int D_700031D8[4];
extern float D_70003A20[4];
extern float D_00810360[4];
extern void func_001026A0(void *dst, void *a, void *b);
extern void func_001028B8(void *dst, void *a, void *b);
extern int func_0019AA80(void *a, void *b, int c);
extern void func_00102948(void *dst, void *src);
extern int func_0019A570(void *a, void *b, int c, int d);
extern void func_001028D0(void *dst, void *a, void *b);
extern float func_00102738(void *a, void *b);
extern void func_001031E0(void *dst, void *src);
extern int func_00122BB8(void);
extern int func_001CD520(int bucket, int mode, void *world, unsigned long long giftag,
                         unsigned int rgba, float w, float h, float zbias);
extern void func_001E2BA0(void *start, void *end, void *colour, float len);

int func_overlay_AREA11_00826EF0(unsigned char *self, void *mtx) {
    Vec4 dir;
    Vec4 tint;
    int hit;
    int save_d8;
    int save_d4;
    int save_d0;
    float d;
    int x;
    int kind;
    int ti;
    float s;

    D_700038A0[0] = 60.0f;
    dir.x = 3.0f;
    dir.y = -2.0f;
    dir.z = 0.0f;
    dir.w = 1.0f;
    D_700038A0[2] = 0.0f;
    D_700038A0[1] = 0.0f;
    D_700038A0[3] = 0.0f;
    func_001026A0(D_700038A0, mtx, D_700038A0);
    func_001026A0(&dir, mtx, &dir);
    func_001028B8(D_700038A0, D_700038A0, &dir);
    D_700038A0[3] = 1.0f;
    if (func_0019AA80(&dir, D_700038A0, 0x20)) {
        func_00102948(D_700038A0, D_700031B0);
        hit = 2;
    } else {
        hit = 0;
    }
    save_d8 = D_700031D8[0];
    save_d4 = D_700031D4[0];
    save_d0 = D_700031D0[0];
    if (func_0019A570(&dir, D_700038A0, 7, 0x20)) {
        func_001028D0(D_700038A0, D_700031B0, D_00810360);
        D_700038A0[3] = 0.0f;
        d = func_00102738(D_700038A0, D_700038A0);
        D_70003A20[0] = d;
        if (d > 10000.0f) {
            hit = 1;
        } else if (D_700031D8[0] == 1 && (kind = (*(unsigned char **)0x700031D4)[3]) >= 0x10 && kind < 0x14) {
            hit = 1;
        } else {
            hit = 2;
        }
    } else {
        D_700031D8[0] = save_d8;
        D_700031D4[0] = save_d4;
        D_700031D0[0] = save_d0;
    }
    if (hit == 2) {
        if (W->f204 == 0 && D_700031D8[0] == 1) {
            W->f204 = D_700031D4[0];
        }
        if (self[4] == 4) {
            func_001031E0(D_700038A0, D_700031B0);
            D_700038A0[3] = 1.0f;
            x = func_00122BB8() >> 16;
            x *= 0xFFFF;
            x >>= 15;
            SPI(0x700038B0) = ((x >> 15) & 0x1F) + 0x40;
            SPI(0x700038B4) = 0;
            SPI(0x700038B8) = 0;
            SPI(0x700038BC) = 0x80;
            ti = 3;
            s = (float)ti;
            func_001CD520(0, 2, D_700038A0, 0x20045BA5154222DCULL,
                          SPI(0x700038BC) << 24 | SPI(0x700038B8) << 16 |
                          SPI(0x700038B4) << 8 | SPI(0x700038B0), 3.0f, s, 2.0f);
            D_700038C0[0] = 0.8f;
            D_700038C0[2] = 0.0f;
            D_700038C0[1] = 0.0f;
            func_001E2BA0(&dir, D_700038A0, D_700038C0, 100.0f);
        } else if (*(int *)(self + 0x200) >= 0xD) {
            tint = D_overlay_AREA11_0082A730[0];
            func_001031E0(D_700038A0, D_700031B0);
            D_700038A0[3] = 1.0f;
            func_001026A0(D_700038C0, *(char **)(D_00275B40 + 0xC) + 0x90, &tint);
            D_700038C0[3] = 1.0f;
            D_700038D0[2] = 0.8f;
            D_700038D0[1] = 0.8f;
            D_700038D0[0] = 0.8f;
            func_001E2BA0(D_700038A0, D_700038C0, D_700038D0, 100.0f);
        }
    } else if (hit == 1) {
        func_001031E0(D_700038B0, D_700031B0);
        D_700038B0[3] = 1.0f;
        D_700038C0[0] = 0.8f;
        D_700038C0[2] = 0.0f;
        D_700038C0[1] = 0.0f;
        func_001E2BA0(&dir, D_700038B0, D_700038C0, 100.0f);
    }
    if (hit == 2) {
        if (W->f204 == D_700031D4[0]) {
            return 2;
        }
        return 1;
    }
    return 0;
}

// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// AREA01 overlay, runtime 0x008282F0 (splat/link name 008282B0; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: probe called from 0x826D40 with the record B matrix (+0x90) as a1.
//  Result code: 0 when func_0019AA80 found nothing and func_0019A570 fails
//  (0x700031D0..D8 are then restored); 1 when func_0019A570 succeeds and either
//  func_00102738 of the offset is above 10000.0 or the hit is kind 1 (0x700031D8)
//  with byte +3 of 0x700031D4's record in 0x10..0x13; 2 otherwise. For 2 it
//  stores 0x700031D4 into +0x204 when +0x204 is 0 and the kind is 1, then calls
//  func_001CD520 + func_001E2BA0 (state byte +4 == 4) or func_001E2BA0 with the
//  0x82CB20 colour (+0x200 >= 13); for 1 it calls func_001E2BA0. Returns 0, or
//  for code 2: 2 when +0x204 equals 0x700031D4, else 1.
// Scratchpad accesses are relocated externs (MATCHING_GUIDE idiom-32) except
// the four integer colour words at 0x700038B0 and the 0x700031D4 record read in
// the classification, which match only as literal addresses.
// Covers the splat pieces 008282B0, 008282F0 (the later piece
// is absorbed at link time, tools/overlay/fill_overlay.py).
typedef struct { float x, y, z, w; } Vec4;
typedef struct {
    float f1F0, f1F4, f1F8, f1FC;
    int f200, f204;
} Work;
#define W ((Work *)(self + 0x1F0))
#define SPI(a) (*(int *)(a))
extern char *D_00275B40;
extern Vec4 D_overlay_AREA01_0082CB20[1];
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
                         float w, float h, float zbias, unsigned int rgba);
extern void func_001E2BA0(void *start, void *end, void *colour, float len);

int func_overlay_AREA01_008282B0(unsigned char *self, void *mtx) {
    Vec4 dir;
    Vec4 tint;
    int hit;
    int save_d8;
    int save_d4;
    int save_d0;
    float d;
    int x;
    int kind;

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
            func_001CD520(0, 2, D_700038A0, 0x20045BA5154222DCULL, 3.0f, 3.0f, 2.0f,
                          SPI(0x700038BC) << 24 | SPI(0x700038B8) << 16 |
                          SPI(0x700038B4) << 8 | SPI(0x700038B0));
            D_700038C0[0] = 0.8f;
            D_700038C0[2] = 0.0f;
            D_700038C0[1] = 0.0f;
            func_001E2BA0(&dir, D_700038A0, D_700038C0, 100.0f);
        } else if (*(int *)(self + 0x200) >= 0xD) {
            tint = D_overlay_AREA01_0082CB20[0];
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

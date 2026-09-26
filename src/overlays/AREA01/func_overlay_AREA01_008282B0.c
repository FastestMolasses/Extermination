// NEARMISS func_overlay_AREA01_008282B0  (runtime 0x008282F0, 0x4D0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 89.86% via mwccps2 2.3.3 (tools/overlay/overlay_match.py check AREA01 <this file>).
// Not compiled by tools/overlay/compile_overlay_src.py; the overlay links this
// function from its splat pieces. Splat names overlay code 0x40 below its runtime
// address (the MWo3 header is loaded first).
// O4 aim/line-of-sight probe called from 0x826D40: casts a ray from the head matrix
// (a1), classifies the hit (0 none, 1 in range, 2 blocked/other) via func_0019AA80 /
// func_0019A570, draws the beam (func_001E2BA0) and a flare (func_001CD520) and
// returns 2/1 depending on whether the hit actor is the stored target (+0x204).
// DIVERGENCE: branch layout and delay-slot filling. The original keeps unfilled
// branch slots before literal scratchpad accesses and a different join layout for
// the hit classification; relocated-scratchpad spellings scored lower (54-75%).
// The hit classification and final return were reconstructed from the branch
// structure and should be re-checked against an oracle before the port relies on it.
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
typedef struct { float x, y, z, w; } Vec4;
#define SPF(a) (*(float *)(a))
#define SPI(a) (*(int *)(a))
#define SPV(a) (*(volatile int *)(a))
extern char *D_00275B40;
extern Vec4 D_overlay_AREA01_0082CB20[1];
extern float D_700038A0[4];
extern float D_700038B0[4];
extern float D_700038C0[4];
extern float D_700038D0[4];
extern float D_700031B0[4];
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
    unsigned char *talk;
    int r;

    SPF(0x700038A0) = 60.0f;
    dir.x = 3.0f;
    dir.y = -2.0f;
    dir.z = 0.0f;
    dir.w = 1.0f;
    SPF(0x700038A8) = 0.0f;
    SPF(0x700038A4) = 0.0f;
    SPF(0x700038AC) = 0.0f;
    func_001026A0(D_700038A0, mtx, D_700038A0);
    func_001026A0(&dir, mtx, &dir);
    func_001028B8(D_700038A0, D_700038A0, &dir);
    SPF(0x700038AC) = 1.0f;
    hit = 0;
    if (func_0019AA80(&dir, D_700038A0, 0x20) != 0) {
        func_00102948(D_700038A0, D_700031B0);
        hit = 2;
    }
    save_d8 = SPI(0x700031D8);
    save_d4 = SPI(0x700031D4);
    save_d0 = SPI(0x700031D0);
    if (func_0019A570(&dir, D_700038A0, 7, 0x20) != 0) {
        func_001028D0(D_700038A0, D_700031B0, D_00810360);
        SPF(0x700038AC) = 0.0f;
        d = func_00102738(D_700038A0, D_700038A0);
        SPF(0x70003A20) = d;
        if (d <= 10000.0f) {
            hit = 1;
            if (SPI(0x700031D8) != 1 || (*(unsigned char **)0x700031D4)[3] < 0x10 ||
                (*(unsigned char **)0x700031D4)[3] >= 0x14) {
                hit = 2;
            }
        } else {
            hit = 1;
        }
    } else {
        SPI(0x700031D8) = save_d8;
        SPI(0x700031D4) = save_d4;
        SPI(0x700031D0) = save_d0;
    }
    if (hit == 2) {
        talk = self + 0x1F0;
        if (*(int *)(self + 0x204) == 0 && SPI(0x700031D8) == 1) {
            *(int *)(talk + 0x14) = SPI(0x700031D4);
        }
        if (self[4] == 4) {
            func_001031E0(D_700038A0, D_700031B0);
            SPF(0x700038AC) = 1.0f;
            x = func_00122BB8() >> 16;
            x *= 0xFFFF;
            x >>= 15;
            SPV(0x700038B0) = ((x >> 15) & 0x1F) + 0x40;
            SPV(0x700038B4) = 0;
            SPV(0x700038B8) = 0;
            SPV(0x700038BC) = 0x80;
            func_001CD520(0, 2, D_700038A0, 0x20045BA5154222DCULL, 3.0f, 3.0f, 2.0f,
                          SPV(0x700038BC) << 24 | SPV(0x700038B8) << 16 |
                          SPV(0x700038B4) << 8 | SPV(0x700038B0));
            SPF(0x700038C0) = 0.8f;
            SPF(0x700038C8) = 0.0f;
            SPF(0x700038C4) = 0.0f;
            func_001E2BA0(&dir, D_700038A0, D_700038C0, 100.0f);
        } else if (*(int *)(self + 0x200) >= 0xD) {
            tint = D_overlay_AREA01_0082CB20[0];
            func_001031E0(D_700038A0, D_700031B0);
            SPF(0x700038AC) = 1.0f;
            func_001026A0(D_700038C0, *(char **)(D_00275B40 + 0xC) + 0x90, &tint);
            SPF(0x700038CC) = 1.0f;
            SPF(0x700038D8) = 0.8f;
            SPF(0x700038D4) = 0.8f;
            SPF(0x700038D0) = 0.8f;
            func_001E2BA0(D_700038A0, D_700038C0, D_700038D0, 100.0f);
        }
    } else if (hit == 1) {
        func_001031E0(D_700038B0, D_700031B0);
        SPF(0x700038BC) = 1.0f;
        SPF(0x700038C0) = 0.8f;
        SPF(0x700038C8) = 0.0f;
        SPF(0x700038C4) = 0.0f;
        func_001E2BA0(&dir, D_700038B0, D_700038C0, 100.0f);
    }
    if (hit == 2) {
        r = 2;
        if (*(int *)(self + 0x204) != SPI(0x700031D4)) {
            r = 1;
        }
        return r;
    }
    return 0;
}

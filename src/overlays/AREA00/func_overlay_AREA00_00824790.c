// NEARMISS func_overlay_AREA00_00824790  (runtime 0x008247D0, 0x3E0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.74% via mwccps2 2.3.3 (tools/overlay/overlay_match.py check AREA00 <this file>).
// Not compiled by tools/overlay/compile_overlay_src.py; the overlay links this
// function from its splat piece. Splat names overlay code 0x40 below its runtime
// address (the MWo3 header is loaded first).
// Role: state 0 builds the +0xD0 matrix, points +0x30 at the block of the
//  record at +0x14 and +0x34 at 0x8247C0 (the runtime address of the empty
//  function func_overlay_AREA00_00824780, written D_008247C0 here), seeds a
//  velocity (0, 0, 2/9) through the matrix and a random seed; states 1 and 2
//  scale 8 * min(t, 1), submit two packets (tables 0x828A60 / 0x828AF0), move
//  by the velocity and stop (state 2, then 3) at t > 1.2 or on
//  func_0019A570(block + 0x30, +0xB0, 6, 0).
// Residual: one branch delay slot: after the min(t, 1) test the original
//  leaves a nop where mwcc 2.3.3 hoists the next constant load.
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
typedef struct {
    float scale[3];     /* 0x00 */
    int pad0C;          /* 0x0C */
    int pad10;          /* 0x10 */
    int seed;           /* 0x14 */
    float t;            /* 0x18 */
    float t2;           /* 0x1C */
    float vel[4];       /* 0x20 */
    float pos[4];       /* 0x30 */
    float mtx[12];      /* 0x40 */
    float pos2[4];      /* 0x70 */
} Fx;
extern float D_70003A20;
extern char D_008247C0[];
extern char D_overlay_AREA00_00828A60[];
extern char D_overlay_AREA00_00828AF0[];
extern void func_001029C0(void *m);
extern void func_00102C58(void *dst, void *a, void *b);
extern void func_00102918(void *a, void *b, void *c);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_00102948(void *dst, void *src);
extern void func_00102958(void *dst, void *src);
extern int func_00122BB8(void);
extern int func_001CCF70(void *p);
extern void func_001CFAE0(void *dst, int a1, void *src, float f12, float f13, float f14, float f15);
extern void func_001CFBE0(int a0, int a1, void *a2, void *a3, int t0);
extern int func_0019A570(void *a, void *b, int c, int d);
extern void func_001B17A0(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA00_00824790(unsigned char *self) {
    int pkt[24];
    int h;
    int seed;
    Fx *fx = (Fx *)(self + 0x1F0);
    unsigned char *link = *(unsigned char **)(self + 0x14) + 0x1F0;
    float r;
    float d;
    switch (self[4]) {
    case 0:
        func_001029C0(self + 0xD0);
        func_00102C58(self + 0xD0, self + 0xD0, self + 0xC0);
        func_00102918(self + 0xD0, self + 0xD0, self + 0xB0);
        self[0] = 1;
        *(unsigned char **)(self + 0x30) = link;
        *(char **)(self + 0x34) = D_008247C0;
        fx->vel[0] = 0.0f;
        fx->vel[1] = 0.0f;
        fx->vel[2] = 0.22222222f;
        fx->vel[3] = 1.0f;
        func_001026A0(fx->vel, self + 0xD0, fx->vel);
        func_001028D0(fx->vel, fx->vel, self + 0xB0);
        func_00102948(fx->pos, self + 0xB0);
        func_00102958(fx->mtx, self + 0xD0);
        fx->seed = func_00122BB8();
        fx->t = 0.0f;
        fx->t2 = 0.0f;
        self[4] = 1;
    case 1:
    case 2:
        *(float *)(self + 0x100) = *(float *)(self + 0xB0);
        *(float *)(self + 0x104) = *(float *)(self + 0xB4);
        *(float *)(self + 0x108) = *(float *)(self + 0xB8);
        D_70003A20 = fx->t;
        d = fx->t;
        if (!(d <= 1.0f)) {
            d = 1.0f;
        }
        D_70003A20 = d;
        fx->scale[0] = 8.0f * D_70003A20;
        fx->scale[1] = 8.0f * D_70003A20;
        fx->scale[2] = 8.0f * D_70003A20;
        seed = fx->seed;
        h = func_001CCF70(self + 0x100);
        r = (float)((seed >> 16) & 0xFFFF);
        r = r / 65535.0f;
        r += 0.0001f;
        seed = seed * 37 + 11;
        func_001CFAE0(pkt, 0, self + 0xD0, fx->t, r, 1.0f, 1e-6f);
        func_001CFBE0(h, 6, D_overlay_AREA00_00828A60, pkt, 0);
        fx->t += 0.006666667f;
        if (!(fx->t <= 1.2f)) {
            self[4] = 2;
        }
        if (func_0019A570(fx->pos, self + 0xB0, 6, 0) != 0) {
            self[4] = 2;
        }
        func_00102948(fx->pos, self + 0xB0);
        *(float *)(self + 0xB0) += fx->vel[0];
        *(float *)(self + 0xB4) += fx->vel[1];
        *(float *)(self + 0xB8) += fx->vel[2];
        if (self[4] == 1) {
            func_001B17A0(self);
        }
        if (fx->t2 < 1.5f) {
            h = func_001CCF70(fx->pos2);
            r = (float)((seed >> 16) & 0xFFFF);
            r = r / 65535.0f;
            r += 0.0001f;
            func_001CFAE0(pkt, 0, fx->mtx, fx->t2, r, 1.0f, 1e-6f);
            func_001CFBE0(h, 1, D_overlay_AREA00_00828AF0, pkt, 0);
            fx->t2 += 0.015f;
        } else if (self[4] == 2) {
            self[4] = 3;
        }
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}

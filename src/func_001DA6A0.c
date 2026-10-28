// NEARMISS func_001DA6A0  (vram 0x001DA6A0, 0x5E8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.97% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful.
// Matching wave 2026-10-28 (was 84.63%): the three probe clip tests are the original inline VU0 code (camera rows loaded into VU0 24..27 once, then per probe a transform, clip against w and clip-flag read), written as one-instruction asm statements in a static inline helper; 128.0f / scale is one expression; func_001D9EE0 is called as (self, unit) — under the EE ABI the float goes to $f12 whatever its position, and the original sets the pointer argument first.
// inline VU0 asm (user-approved 2026-10-28 for the VU0 class)
// Remaining differences: one addu operand order in the bone loop (the original adds the scaled index to self, mwcc emits self + index for every spelling tried: index-first sums, int casts, array-of-pointer indexing, unsigned index).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

typedef unsigned char u8;

typedef struct { float x, y, z, w; } Vec4;
typedef struct { Vec4 r[4]; }        Mtx4;

extern char  *D_00275B40;                 /* -> table of global object pointers   */
extern char  *D_00275688;                 /* active camera/view record            */
extern char *D_0028A56C[];                /* [0] = resource archive               */

extern const Vec4 D_0026E620;             /* { 0, -40, 0, 0 } probe offset        */
extern const Vec4 D_0026E630;             /* { 0,   0, 0, 0 } probe offset        */
extern const Vec4 D_0026E640;             /* { 0,  -5, 0, 0 } probe offset        */
extern const Vec4 D_00253210;             /* { 128,   0, 0,   1 } RGBA            */
extern const Vec4 D_00253220;             /* {   0, 128, 0, 128 } RGBA            */
extern const Vec4 D_00253230;             /* {   0,   0, 0,   0 } rotation        */

extern char D_00817FA0[0x10];             /* farthest-bone transform slot         */
extern char D_00817FB0[0x10];             /* nearest-bone transform slot          */
extern char D_00817FF0[0x10];             /* reference transform for func_001DA080 */

extern void  func_00102948(void *dst, const void *src);           /* qword copy   */
extern void  func_001028B8(void *dst, const void *a, const void *b); /* dst=a+b   */
extern void  func_001029C0(void *m);                              /* m = identity */
extern void  func_00102BB0(void *dst, const void *src, float ang);/* rotate       */
extern float func_0011E748(float x);                              /* sqrtf        */
extern char *func_001CD370(int which);
extern char *func_001C6120(char *archive, int id);
extern void  func_001D98A0(void *pos, int variant, float unit);
extern void  func_001DA080(void *near_out, void *far_out, char *self, void *ref);
extern void  func_001DA290(int a, int b);
extern void  func_001D1F80(int a, int b, int c);
extern void  func_001D1F20(int a);
extern void  func_001DA310(void *pos, const void *rot, const void *rgba,
                           char *res, float size);
extern void  func_001D9EE0(char *self, float unit);
extern void  func_001D5C80(void *a, void *b);

/* Clip p on VU0 against the matrix rows held in VU0 registers 24..27:
   transform, clip against w, wait for the flag pipeline, and return the
   six clip flags. */
static inline int vu0_clip(const Vec4 *p)
{
    int c;

    asm("lqc2 vf1, 0x0(%0)" : : "r"(p));
    asm("vmulax.xyzw ACC, vf24, vf1x");
    asm("vmadday.xyzw ACC, vf25, vf1y");
    asm("vmaddaz.xyzw ACC, vf26, vf1z");
    asm("vmaddw.xyzw vf2, vf27, vf0w");
    asm("vclipw.xyz vf2, vf2w");
    asm("vnop");
    asm("vnop");
    asm("vnop");
    asm("vnop");
    asm("vnop");
    asm("cfc2.ni %0, vi18" : "=r"(c));
    return c & 0x3F;
}

void func_001DA6A0(char *self)
{
    Vec4  pos;              /* sp+0x40 */
    Vec4  probe[3];         /* sp+0x50, +0x60, +0x70 */
    Mtx4  spin;             /* sp+0x80 — built and never read               */
    Vec4  colA;             /* sp+0xC0 */
    Vec4  colB;             /* sp+0xD0 */
    Vec4  rot;              /* sp+0xE0 */
    const Mtx4 *cam;
    short kind;
    int   variant;
    int   sub;
    int   i;
    int   flags;
    float scale;
    float unit;
    float yaw;
    float x, z;
    float minx_a, minx_b, minz_a, minz_b;
    float d;

    variant = 0;
    scale   = 16.0f;
    unit    = 128.0f / scale;               /* 8.0f */

    kind = *(short *)(self + 0x96);
    if (kind == 0) {
        return;
    }

    switch (*(short *)(self + 0x96)) {
    case 0x28:
        sub = *(u8 *)(self + 0x98);
        if (sub == 0xFF) {
            func_00102948(&pos, self + 0xB0);
        } else {
            func_00102948(&pos, *(char **)(D_00275B40 + sub * 4) + 0xC0);
        }
        if (*(u8 *)(self + 0x23C) != 0) {
            variant = 1;
        }
        break;
    case 0x29:
    default:
        func_00102948(&pos, *(char **)(D_00275B40 + 0xC) + 0xC0);
        break;
    case 0x2A:
        func_00102948(&pos, *(char **)(D_00275B40 + 0xC) + 0xC0);
        break;
    case 0x2B:
        func_00102948(&pos, *(char **)(D_00275B40 + 0xC) + 0xC0);
        break;
    case 0x2C:
        func_00102948(&pos, *(char **)(D_00275B40 + 0xC) + 0xC0);
        break;
    case 0x2D:
        func_00102948(&pos, *(char **)(D_00275B40 + 0xC) + 0xC0);
        break;
    case 0x2E:
        func_00102948(&pos, *(char **)(D_00275B40 + 8) + 0xC0);
        break;
    case 0x2F:
        func_00102948(&pos, *(char **)(D_00275B40 + 8) + 0xC0);
        break;
    case 0x30:
        func_00102948(&pos, *(char **)(D_00275B40 + 8) + 0xC0);
        break;
    case 0x31:
        func_00102948(&pos, *(char **)(D_00275B40 + 8) + 0xC0);
        break;
    case 0x32:
        func_00102948(&pos, *(char **)(D_00275B40 + 0xC) + 0xC0);
        break;
    case 0x33:
        func_00102948(&pos, *(char **)(D_00275B40 + 0xC) + 0xC0);
        break;
    case 0x34:
        func_00102948(&pos, *(char **)(D_00275B40 + 8) + 0xC0);
        break;
    }
    pos.w = 1.0f;

    switch (*(short *)(self + 0x96)) {
    case 0x28: case 0x29: case 0x2A: case 0x2B: case 0x2C: case 0x2D:
    case 0x2F: case 0x30: case 0x31: case 0x32: case 0x33: case 0x34:
    default:
        func_00102948(&probe[0], &pos);
        func_001028B8(&probe[1], &pos, &D_0026E620);
        func_001028B8(&probe[2], &pos, &D_0026E630);
        break;
    case 0x2E:
        func_00102948(&probe[0], &pos);
        func_001028B8(&probe[1], &pos, &D_0026E620);
        func_001028B8(&probe[2], &pos, &D_0026E640);
        break;
    }

    cam = (const Mtx4 *)func_001CD370(0);
    /* load the camera matrix rows into VU0 registers 24..27 */
    asm("lqc2 vf24, 0x0(%0)" : : "r"(cam));
    asm("lqc2 vf25, 0x10(%0)" : : "r"(cam));
    asm("lqc2 vf26, 0x20(%0)" : : "r"(cam));
    asm("lqc2 vf27, 0x30(%0)" : : "r"(cam));
    flags = vu0_clip(&probe[0]);
    flags &= vu0_clip(&probe[1]);
    flags &= vu0_clip(&probe[2]);
    if (flags != 0) {
        return;
    }

    minx_a = *(float *)(*(char **)(self + 0x114) + 0xC0);
    minx_b = minx_a;
    minz_a = *(float *)(*(char **)(self + 0x114) + 0xC8);
    minz_b = minz_a;
    for (i = 2; i < (int)*(u8 *)(self + 9); i++) {
        char *node = *(char **)(i * 4 + self + 0x110);
        x = *(float *)(node + 0xC0);
        z = *(float *)(node + 0xC8);

        minx_a = (minx_a < x) ? minx_a : x;
        minx_b = (minx_b < x) ? minx_b : x;
        minz_a = (minz_a < z) ? minz_a : z;
        minz_b = (minz_b < z) ? minz_b : z;
    }
    d = (minx_b - minx_a) * (minx_b - minx_a) +
        (minz_b - minz_a) * (minz_b - minz_a);
    d = func_0011E748(d);
    if (!(d <= 7.0f)) {
        scale = 1.0f + 2.0f * d;
    }

    func_001D98A0(&pos, variant, unit);
    func_001DA080(D_00817FB0, D_00817FA0, self, D_00817FF0);

    yaw = *(float *)(D_00275688 + 0xB4);
    func_001029C0(&spin);
    func_00102BB0(&spin, &spin, yaw);

    func_001DA290(0, 0);
    colA = D_00253210;
    colB = D_00253220;
    rot  = D_00253230;

    func_001D1F80(0, 2, 9);
    func_001DA310(&pos, &rot, &colB, func_001C6120(D_0028A56C[0], 0x14), scale);
    func_001DA310(&pos, &rot, &colA, func_001C6120(D_0028A56C[0], 0x15), scale);
    func_001D1F20(0);

    func_001D9EE0(self, unit);
    func_001D5C80(D_00817FB0, D_00817FA0);
}

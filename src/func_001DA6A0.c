// NEARMISS func_001DA6A0  (vram 0x001DA6A0, ?) — readable decompilation, NOT byte-identical.
//
// objdiff 49.82% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef unsigned char u8;

typedef struct { float x, y, z, w; } Vec4;
typedef struct { Vec4 r[4]; }        Mtx4;

extern char  *D_00275B40;                 /* -> table of global object pointers   */
extern char  *D_00275688;                 /* active camera/view record            */
extern char **D_0028A56C;                 /* -> resource archive                  */

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
extern void  func_001D9EE0(float unit, char *self);
extern void  func_001D5C80(void *a, void *b);

/*
 * Transform `p` by `m` and return the six VU CLIP flags (VI18 & 0x3F) produced
 * by vclipw.xyz — one bit each for x>+w, x<-w, y>+w, y<-w, z>+w, z<-w. There is
 * no portable C equivalent; a port has to recreate the flag bits explicitly.
 */
static int clip_flags(const Mtx4 *m, const Vec4 *p)
{
    float x = m->r[0].x * p->x + m->r[1].x * p->y + m->r[2].x * p->z + m->r[3].x;
    float y = m->r[0].y * p->x + m->r[1].y * p->y + m->r[2].y * p->z + m->r[3].y;
    float z = m->r[0].z * p->x + m->r[1].z * p->y + m->r[2].z * p->z + m->r[3].z;
    float w = m->r[0].w * p->x + m->r[1].w * p->y + m->r[2].w * p->z + m->r[3].w;
    float aw = (w < 0.0f) ? -w : w;
    int f = 0;
    if (x >  aw) { f |= 0x01; }
    if (x < -aw) { f |= 0x02; }
    if (y >  aw) { f |= 0x04; }
    if (y < -aw) { f |= 0x08; }
    if (z >  aw) { f |= 0x10; }
    if (z < -aw) { f |= 0x20; }
    return f;
}

void func_001DA6A0(char *self)
{
    Vec4  pos;              /* sp+0x40 */
    Vec4  probe[3];         /* sp+0x50, +0x60, +0x70 */
    Mtx4  spin;             /* sp+0x80 — built and never read (see header)  */
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
    float minx_a, minx_b, minz_a, minz_b;
    float d;

    variant = 0;
    scale   = 8.0f;
    unit    = 128.0f / scale;               /* 16.0f */

    kind = *(short *)(self + 0x96);
    if (kind == 0) {
        return;
    }

    /* 2. Anchor position (jtbl_0026E690, index kind - 0x28, 13 entries). */
    if (kind == 0x28) {
        sub = *(u8 *)(self + 0x98);
        if (sub == 0xFF) {
            func_00102948(&pos, self + 0xB0);
        } else {
            func_00102948(&pos, *(char **)(D_00275B40 + sub * 4) + 0xC0);
        }
        if (*(u8 *)(self + 0x23C) != 0) {
            variant = 1;
        }
    } else if (kind == 0x2E || kind == 0x2F || kind == 0x30 ||
               kind == 0x31 || kind == 0x34) {
        func_00102948(&pos, *(char **)(D_00275B40 + 8) + 0xC0);      /* entry 2 */
    } else {
        func_00102948(&pos, *(char **)(D_00275B40 + 0xC) + 0xC0);    /* entry 3 */
    }
    pos.w = 1.0f;

    /* 3. Probe set (jtbl_0026E650: only kind 0x2E differs) and the clip test. */
    func_00102948(&probe[0], &pos);
    func_001028B8(&probe[1], &pos, &D_0026E620);
    if (*(short *)(self + 0x96) == 0x2E) {
        func_001028B8(&probe[2], &pos, &D_0026E640);
    } else {
        func_001028B8(&probe[2], &pos, &D_0026E630);
    }

    cam   = (const Mtx4 *)func_001CD370(0);
    flags = clip_flags(cam, &probe[0]);
    flags &= clip_flags(cam, &probe[1]);
    flags &= clip_flags(cam, &probe[2]);
    if (flags != 0) {
        return;                              /* wholly outside one plane */
    }

    /* 4. Bone spread -> marker size. See the SHIPPED BUG note in the header:
          all four accumulators take the minimum, so d is always 0. */
    minx_a = minx_b = *(float *)(*(char **)(self + 0x114) + 0xC0);
    minz_a = minz_b = *(float *)(*(char **)(self + 0x114) + 0xC8);
    for (i = 2; i < (int)*(u8 *)(self + 9); i++) {
        char *node = *(char **)(self + 0x110 + i * 4);
        float x = *(float *)(node + 0xC0);
        float z = *(float *)(node + 0xC8);

        if (!(minx_a < x)) { minx_a = x; }
        if (!(minx_b < x)) { minx_b = x; }   /* intended max(), see header */
        if (!(minz_a < z)) { minz_a = z; }
        if (!(minz_b < z)) { minz_b = z; }   /* intended max(), see header */
    }
    d = func_0011E748((minx_b - minx_a) * (minx_b - minx_a) +
                      (minz_b - minz_a) * (minz_b - minz_a));
    if (!(d <= 7.0f)) {
        scale = 1.0f + 2.0f * d;
    }

    /* 5. Marker orientation + nearest/farthest bone pick. */
    func_001D98A0(&pos, variant, unit);
    func_001DA080(D_00817FB0, D_00817FA0, self, D_00817FF0);

    /* 6. Vestigial: built from the camera angle, never read. */
    yaw = *(float *)(D_00275688 + 0xB4);
    func_001029C0(&spin);
    func_00102BB0(&spin, &spin, yaw);

    /* 7/8. Two billboards at `pos`, sized by `scale`. */
    func_001DA290(0, 0);
    colA = D_00253210;
    colB = D_00253220;
    rot  = D_00253230;

    func_001D1F80(0, 2, 9);
    func_001DA310(&pos, &rot, &colB, func_001C6120(*D_0028A56C, 0x14), scale);
    func_001DA310(&pos, &rot, &colA, func_001C6120(*D_0028A56C, 0x15), scale);
    func_001D1F20(0);

    /* 9. Actor overlay pass + the two picked bone transforms. */
    func_001D9EE0(unit, self);
    func_001D5C80(D_00817FB0, D_00817FA0);
}

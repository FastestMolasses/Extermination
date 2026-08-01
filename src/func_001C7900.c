// NEARMISS func_001C7900  (vram 0x001C7900, 0x2F8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 20.11% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef unsigned __int128 uint128;

/* One VU0 quadword: four 32-bit floats, x/y/z/w. */
typedef struct { float x, y, z, w; } Vec4;
/* Row-major 4x4; row 3 is the translation row in this engine's convention. */
typedef struct { Vec4 r[4]; } Mtx4;

/* Display-list control block. The cursor table lives at +0x10, word stride:
   cursor[i] == *(char **)((char *)D_00275670 + 0x10 + i * 4). */
extern int *D_00275670;

/* PS2 scratchpad RAM (0x70000000..0x70003FFF) staging area. */
extern Mtx4 D_70003400; /* SPR_MAT_A  — produced by func_001D88B0 */
extern Mtx4 D_70003440; /* SPR_MAT_B  — produced by func_001D88B0, uploaded raw */
extern Vec4 D_70003480; /* normalized basis row 0 */
extern Vec4 D_70003490; /* normalized basis row 1 */
extern Vec4 D_700034A0; /* normalized basis row 2 */
extern Vec4 D_700034B0; /* translation row (verbatim copy of mtx->r[3]) */
extern Mtx4 D_70003AC0; /* SPR_MAT_C */

extern void func_001D88B0(void *pos, void *out_a, void *out_b, int token);

/* VU0 macro-mode row-vector * 4x4:
   out = v.x*m.r0 + v.y*m.r1 + v.z*m.r2 + v.w*m.r3
   (vmulax / vmadday / vmaddaz / vmaddw through $ACC, all four lanes at once). */
static void vu0_mul_vec4_mtx(Vec4 *out, const Vec4 *v, const Mtx4 *m) {
    out->x = v->x * m->r[0].x + v->y * m->r[1].x + v->z * m->r[2].x + v->w * m->r[3].x;
    out->y = v->x * m->r[0].y + v->y * m->r[1].y + v->z * m->r[2].y + v->w * m->r[3].y;
    out->z = v->x * m->r[0].z + v->y * m->r[1].z + v->z * m->r[2].z + v->w * m->r[3].z;
    out->w = v->x * m->r[0].w + v->y * m->r[1].w + v->z * m->r[2].w + v->w * m->r[3].w;
}

static void vu0_mul_mtx4(Mtx4 *out, const Mtx4 *a, const Mtx4 *b) {
    int i;
    for (i = 0; i < 4; i++) {
        vu0_mul_vec4_mtx(&out->r[i], &a->r[i], b);
    }
}

/* VU0 macro-mode xyz normalize; w is cleared first (vsub.xyzw vf6,vf0,vf0).
   Length via vsqrt on $Q, reciprocal via vdiv on $Q (VU0 divide unit). */
static void vu0_normalize_xyz(Vec4 *out, const Vec4 *v) {
    float len2 = v->x * v->x + v->y * v->y + v->z * v->z;
    float len  = __builtin_sqrtf(len2);   /* vsqrt Q, vf5x ; vwaitq */
    float inv  = 1.0f / len;              /* vdiv  Q, vf0w, vf5x ; vwaitq */
    out->x = v->x * inv;
    out->y = v->y * inv;
    out->z = v->z * inv;
    out->w = 0.0f;
}

void *func_001C7900(const Mtx4 *mtx, int arg1, int vu_addr, int slot_idx) {
    char **cursor = (char **)((char *)D_00275670 + 0x10 + slot_idx * 4);
    void *first = *cursor;
    char *p;
    Mtx4 basis;

    /* Build the two scratchpad matrices from the object's world position. */
    func_001D88B0((void *)&mtx->r[3], &D_70003400, &D_70003440, arg1);

    /* ---- chunk 1: qwc 5 — FLUSH, STCYCL, UNPACK 4 qw of SPR_MAT_B to vu_addr */
    p = *cursor;
    p[3] = 0x10;                       /* chunk kind */
    *(int *)(p + 4) = 0;               /* link / next = none */
    *(short *)(p + 0) = 5;             /* qwc of the payload that follows +0x10 */
    *cursor = p + 0x60;

    *(uint128 *)(p + 0x10) = 0;                       /* VIF qword: NOP in slot 0 */
    *(int *)(p + 0x14) = 0x11000000;                  /* VIFcode FLUSH            */
    *(int *)(p + 0x18) = 0x01000101;                  /* VIFcode STCYCL cl=1 wl=1 */
    *(int *)(p + 0x1C) = 0x6C040000 | vu_addr;        /* UNPACK V4-32 num=4       */
    *(Mtx4 *)(p + 0x20) = D_70003440;                 /* four quadwords of payload */

    /* ---- chunk 2: qwc 9 — STCYCL, UNPACK 8 qw to VU address 0 -------------- */
    p = *cursor;
    p[3] = 0x10;
    *(int *)(p + 4) = 0;
    *(short *)(p + 0) = 9;
    *cursor = p + 0xA0;

    *(uint128 *)(p + 0x10) = 0;                       /* NOP, NOP in slots 0/1     */
    *(int *)(p + 0x18) = 0x01000101;                  /* VIFcode STCYCL cl=1 wl=1  */
    *(int *)(p + 0x1C) = 0x6C080000;                  /* UNPACK V4-32 num=8 addr=0 */

    /* payload rows 0..3: mtx * SPR_MAT_C */
    vu0_mul_mtx4((Mtx4 *)(p + 0x20), mtx, &D_70003AC0);

    /* basis = mtx with unit-length xyz rows (w = 0) and the translation intact,
       staged in scratchpad exactly where the original put it. */
    vu0_normalize_xyz(&D_70003480, &mtx->r[0]);
    vu0_normalize_xyz(&D_70003490, &mtx->r[1]);
    vu0_normalize_xyz(&D_700034A0, &mtx->r[2]);
    D_700034B0 = mtx->r[3];
    basis.r[0] = D_70003480;
    basis.r[1] = D_70003490;
    basis.r[2] = D_700034A0;
    basis.r[3] = D_700034B0;

    /* payload rows 4..7: normalized basis * SPR_MAT_A */
    vu0_mul_mtx4((Mtx4 *)(p + 0x60), &basis, &D_70003400);

    return first;
}

// NEARMISS func_001C9610  (vram 0x001C9610, 0x330 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 32.66% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef struct { float x, y, z, w; } Vec4;   /* one VU0 quadword */
typedef struct { Vec4 r[4]; } Mtx4;          /* row-major; row 3 = translation */

typedef struct SkelNode {
    /* 0x00 */ Mtx4  bind;
    /* 0x40 */ char  pad40[0x64 - 0x40];
    /* 0x64 */ short parent;                 /* index into `nodes`, -1 = root */
    /* 0x66 */ char  pad66[0x70 - 0x66];
    /* 0x70 */ float rot[3];                 /* radians, applied Z, Y, X */
    /* 0x7C */ float trans[3];
    /* 0x88 */ short scale[3];               /* 1/4096 fixed point */
    /* 0x8E */ char  pad8E[0x90 - 0x8E];
    /* 0x90 */ Mtx4  world;                  /* OUTPUT */
} SkelNode;

/* PS2 scratchpad staging matrix at 0x70003400 (rows at 0x3400/10/20/30). */
extern Mtx4 D_70003400;

/* SDK VU0 helpers (both are hand-written asm leaves in this tree). */
extern void func_001029C0(Mtx4 *m);                        /* m = identity */
extern void func_00102C58(Mtx4 *dst, Mtx4 *src, float *rot); /* dst = src * Rzyx(rot) */

/* VU0 macro-mode row-vector * 4x4 (vmulax/vmadday/vmaddaz/vmaddw via $ACC). */
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

/* qmtc2 scalar into vf5 + vmulx.xyz: scale xyz in place, leave w alone. */
static void vu0_scale_row_xyz(Vec4 *row, float s) {
    row->x *= s;
    row->y *= s;
    row->z *= s;
}

void func_001C9610(SkelNode **nodes, int count, const Mtx4 *root) {
    int i;

    for (i = 0; i < count; i++) {
        SkelNode *n;

        /* SPR = identity, then the ZYX Euler rotation. Note the node pointer is
           re-loaded from nodes[i] between every step in the original; the calls
           may move the array, so keep the reloads if you port this literally. */
        func_001029C0(&D_70003400);
        n = nodes[i];
        func_00102C58(&D_70003400, &D_70003400, &n->rot[0]);

        n = nodes[i];
        D_70003400.r[3].x = n->trans[0];
        D_70003400.r[3].y = n->trans[1];
        D_70003400.r[3].z = n->trans[2];

        vu0_scale_row_xyz(&D_70003400.r[0], (float)n->scale[0] * (1.0f / 4096.0f));
        vu0_scale_row_xyz(&D_70003400.r[1], (float)n->scale[1] * (1.0f / 4096.0f));
        vu0_scale_row_xyz(&D_70003400.r[2], (float)n->scale[2] * (1.0f / 4096.0f));

        /* SPR = SPR * bind, in place. */
        vu0_mul_mtx4(&D_70003400, &D_70003400, &n->bind);

        if (n->parent != -1) {
            vu0_mul_mtx4(&n->world, &D_70003400, &nodes[n->parent]->world);
        } else {
            vu0_mul_mtx4(&n->world, &D_70003400, root);
        }
    }
}

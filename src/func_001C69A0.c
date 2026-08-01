// NEARMISS func_001C69A0  (vram 0x001C69A0, 0x3F8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 23.04% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef struct { float x, y, z, w; } Vec4;

/* Model instance. The bone records themselves are accessed by byte offset
   (see the field map in the header comment) because their surrounding layout
   is not fully mapped yet. */
typedef struct {
    char          pad00[0x0C];
    unsigned char bone_count;   /* +0x0C */
    char          pad0D[0x60 - 0x0D];
    float         scale[4];     /* +0x60 */
    char          pad70[0x110 - 0x70];
    unsigned char *bone[1];     /* +0x110 array of bone pointers */
} Model;

/* Byte-matched / documented helpers. */
extern void quat_nlerp(float *out, const float *qa, const float *qb, float t);
extern void quat_to_mat3(float *m, const float *q, const float *translation);
extern void func_001029C0(Vec4 *m);                              /* identity */
extern void func_00102C58(Vec4 *dst, Vec4 *src, const float *euler_xyz);

/* EE scratchpad working set. */
#define SPR_OBJECT ((Vec4 *)0x70003400)   /* object world matrix, staged by caller */
#define SPR_ANIM   ((Vec4 *)0x70003440)   /* animation matrix                      */
#define SPR_REST   ((Vec4 *)0x70003480)   /* rest/bind matrix, then the combined   */
#define SPR_QUAT   ((float *)0x70003600)  /* blended quaternion                    */

/* --- VU0 macro-mode helpers written out as scalar float math -------------- */

/* `vmulx.xyz vfD, vfD, vfSx` etc: scale a row's xyz by a broadcast scalar,
   leaving w untouched. */
static void vu0_scale_row_xyz(Vec4 *row, float s) {
    row->x = row->x * s;
    row->y = row->y * s;
    row->z = row->z * s;
}

/* vmulax/vmadday/vmaddaz/vmaddw accumulator chain: out = a * b, row-vector
   convention (each row of `a` is transformed by the whole of `b`). */
static void vu0_mat4_mul(Vec4 *out, const Vec4 *a, const Vec4 *b) {
    int i;
    for (i = 0; i < 4; i++) {
        Vec4 r;
        r.x = b[0].x * a[i].x + b[1].x * a[i].y + b[2].x * a[i].z + b[3].x * a[i].w;
        r.y = b[0].y * a[i].x + b[1].y * a[i].y + b[2].y * a[i].z + b[3].y * a[i].w;
        r.z = b[0].z * a[i].x + b[1].z * a[i].y + b[2].z * a[i].z + b[3].z * a[i].w;
        r.w = b[0].w * a[i].x + b[1].w * a[i].y + b[2].w * a[i].z + b[3].w * a[i].w;
        out[i] = r;
    }
}

void func_001C69A0(Model *model) {
    int i;

    /* --- 0. pre-scale the staged object matrix in place -------------------- */
    vu0_scale_row_xyz(&SPR_OBJECT[0], model->scale[0]);
    vu0_scale_row_xyz(&SPR_OBJECT[1], model->scale[1]);
    vu0_scale_row_xyz(&SPR_OBJECT[2], model->scale[2]);
    /* The target also re-stores row 3 unchanged (it loads all four rows and
       writes all four back); kept here so the store pattern is visible. */
    SPR_OBJECT[3] = SPR_OBJECT[3];

    /* --- 1. evaluate every bone ------------------------------------------- */
    for (i = 0; i < (int)model->bone_count; i++) {
        unsigned char *bone = model->bone[i];
        const Vec4 *parent_mtx;
        short parent;

        /* a/b: animated rotation basis from the blended key quaternion */
        quat_nlerp((float *)SPR_QUAT,
                   (const float *)(bone + 0x30),
                   (const float *)(bone + 0x40),
                   *(float *)(bone + 0x50));
        quat_to_mat3((float *)SPR_ANIM, (const float *)SPR_QUAT,
                     (const float *)bone);

        vu0_scale_row_xyz(&SPR_ANIM[0], *(float *)(bone + 0x18));
        vu0_scale_row_xyz(&SPR_ANIM[1], *(float *)(bone + 0x1C));
        vu0_scale_row_xyz(&SPR_ANIM[2], *(float *)(bone + 0x20));

        /* c: the bone's rest (bind) TRS matrix */
        func_001029C0(SPR_REST);                               /* identity  */
        func_00102C58(SPR_REST, SPR_REST, (const float *)(bone + 0x70));

        SPR_REST[3].x = *(float *)(bone + 0x7C);               /* translate */
        SPR_REST[3].y = *(float *)(bone + 0x80);
        SPR_REST[3].z = *(float *)(bone + 0x84);

        /* three s16 rest scales, 4.12 fixed point */
        vu0_scale_row_xyz(&SPR_REST[0],
                          (1.0f / 4096.0f) * (float)*(short *)(bone + 0x88));
        vu0_scale_row_xyz(&SPR_REST[1],
                          (1.0f / 4096.0f) * (float)*(short *)(bone + 0x8A));
        vu0_scale_row_xyz(&SPR_REST[2],
                          (1.0f / 4096.0f) * (float)*(short *)(bone + 0x8C));

        /* d: REST = REST * ANIM (row-vector order: rest first) */
        vu0_mat4_mul(SPR_REST, SPR_REST, SPR_ANIM);

        /* e: concatenate with the parent's world matrix (or the object's) */
        parent = *(short *)(bone + 0x64);
        if (parent == -1) {
            parent_mtx = SPR_OBJECT;
        } else {
            parent_mtx = (const Vec4 *)(model->bone[parent] + 0x90);
        }
        vu0_mat4_mul((Vec4 *)(bone + 0x90), SPR_REST, parent_mtx);
    }
}

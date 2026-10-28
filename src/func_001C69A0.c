// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was a 94.83% NEARMISS).
// Fixes read from the original instructions:
// - The row-scale helper takes the scale as a float and moves it into t0 with
//   an asm float-to-GPR move before the VU0 scalar transfer, replacing the
//   integer reinterpretation / stack union. For a scale stored in memory mwcc
//   turns that move into a direct integer load into t0, as in the original;
//   the stack frame shrinks to the original's size.
// - inline VU0 asm (user-approved 2026-10-28 for the VU0 class, incl. the
//   float-to-GPR move before a VU0 transfer): VU0 macro instructions and the
//   float-to-GPR move are one instruction per asm statement; everything else
//   is C. No explicit nop is written: mwcc inserts the hazard nop after the
//   move itself.
//
// Evaluates a model's skeleton: scales the staged object matrix by the model
// scale, then for every bone builds the animated rotation (quaternion blend
// of the two keys, to a matrix with the animated translation) scaled per row,
// the rest matrix (identity, rest rotation, rest translation, 4.12 rest
// scales), combines rest * animation, and writes the bone's world matrix as
// that local matrix times the parent's world matrix (the object matrix for
// root bones).

typedef struct { float x, y, z, w; } Vec4;

typedef struct Bone {
    float  trans[4];        /* +0x00 animated translation             */
    char   pad10[0x08];
    float  scale[3];        /* +0x18 animated scale                   */
    char   pad24[0x0C];
    float  qa[4];           /* +0x30 key quaternion A                 */
    float  qb[4];           /* +0x40 key quaternion B                 */
    float  t;               /* +0x50 blend factor                     */
    char   pad54[0x10];
    short  parent;          /* +0x64 parent bone, -1 = object root    */
    char   pad66[0x0A];
    float  rest_rot[3];     /* +0x70 rest rotation (euler xyz)        */
    float  rest_trans[3];   /* +0x7C rest translation                 */
    short  rest_scale[3];   /* +0x88 rest scale, 4.12 fixed point     */
    char   pad8E[0x02];
    Vec4   world[4];        /* +0x90 bone world matrix                */
} Bone;

typedef struct {
    char          pad00[0x0C];
    unsigned char bone_count;   /* +0x0C */
    char          pad0D[0x60 - 0x0D];
    Vec4          scale;        /* +0x60 */
    char          pad70[0x110 - 0x70];
    Bone         *bone[1];      /* +0x110 */
} Model;

extern void quat_nlerp(float *out, float *a, float *b, float t);
extern void quat_to_mat3(float *m, float *q, Bone *node);
extern void func_001029C0(Vec4 *m);                       /* identity */
extern void func_00102C58(Vec4 *obj, Vec4 *obj2, float *v); /* obj = rot(v) * obj; its src definition names the 2nd argument int unused */

extern Vec4 D_70003400[4];   /* object world matrix, staged by the caller */
extern Vec4 D_70003440[4];   /* animation matrix */
extern Vec4 D_70003480[4];   /* rest matrix, then the combined local matrix */
extern float D_70003600[4];  /* blended quaternion */

/* dst.xyz = src.xyz * s (s handed to VU0 through t0), dst.w = src.w */
static inline void vu0_scale_xyz(Vec4 *dst, Vec4 *src, float s)
{
    Vec4 *d = dst;
    Vec4 *a = src;

    asm("lqc2 vf4, 0x0(%0)" : : "r"(a));
    asm("mfc1 $8, %0" : : "f"(s));
    asm("qmtc2.ni $8, vf5");
    asm("vmulx.xyz vf4, vf4, vf5x");
    asm("sqc2 vf4, 0x0(%0)" : : "r"(d) : "memory");
}

/* m0 = m1 * m2 (row vectors): each row of m2 transformed by m1 */
static inline void vu0_mul_matrix(Vec4 *m0, Vec4 *m1, Vec4 *m2)
{
    Vec4 *r = m2;
    Vec4 *a = m1;
    Vec4 *d = m0;

    asm("lqc2 vf4, 0x0(%0)" : : "r"(a));
    asm("lqc2 vf5, 0x10(%0)" : : "r"(a));
    asm("lqc2 vf6, 0x20(%0)" : : "r"(a));
    asm("lqc2 vf7, 0x30(%0)" : : "r"(a));
    asm("lqc2 vf8, 0x0(%0)" : : "r"(r));
    asm("vmulax.xyzw ACC, vf4, vf8x");
    asm("vmadday.xyzw ACC, vf5, vf8y");
    asm("vmaddaz.xyzw ACC, vf6, vf8z");
    asm("vmaddw.xyzw vf12, vf7, vf8w");
    asm("sqc2 vf12, 0x0(%0)" : : "r"(d) : "memory");
    asm("lqc2 vf9, 0x10(%0)" : : "r"(r));
    asm("vmulax.xyzw ACC, vf4, vf9x");
    asm("vmadday.xyzw ACC, vf5, vf9y");
    asm("vmaddaz.xyzw ACC, vf6, vf9z");
    asm("vmaddw.xyzw vf13, vf7, vf9w");
    asm("sqc2 vf13, 0x10(%0)" : : "r"(d) : "memory");
    asm("lqc2 vf10, 0x20(%0)" : : "r"(r));
    asm("vmulax.xyzw ACC, vf4, vf10x");
    asm("vmadday.xyzw ACC, vf5, vf10y");
    asm("vmaddaz.xyzw ACC, vf6, vf10z");
    asm("vmaddw.xyzw vf14, vf7, vf10w");
    asm("sqc2 vf14, 0x20(%0)" : : "r"(d) : "memory");
    asm("lqc2 vf11, 0x30(%0)" : : "r"(r));
    asm("vmulax.xyzw ACC, vf4, vf11x");
    asm("vmadday.xyzw ACC, vf5, vf11y");
    asm("vmaddaz.xyzw ACC, vf6, vf11z");
    asm("vmaddw.xyzw vf15, vf7, vf11w");
    asm("sqc2 vf15, 0x30(%0)" : : "r"(d) : "memory");
}

/* scale the xyz of rows 0..2 of src by v.x, v.y, v.z into dst */
static inline void vu0_scale_matrix(Vec4 *dst, Vec4 *src, Vec4 *v)
{
    Vec4 *d = dst;
    Vec4 *a = src;
    Vec4 *b = v;

    asm("lqc2 vf4, 0x0(%0)" : : "r"(a));
    asm("lqc2 vf5, 0x10(%0)" : : "r"(a));
    asm("lqc2 vf6, 0x20(%0)" : : "r"(a));
    asm("lqc2 vf7, 0x30(%0)" : : "r"(a));
    asm("lqc2 vf8, 0x0(%0)" : : "r"(b));
    asm("vmulx.xyz vf4, vf4, vf8x");
    asm("vmuly.xyz vf5, vf5, vf8y");
    asm("vmulz.xyz vf6, vf6, vf8z");
    asm("sqc2 vf4, 0x0(%0)" : : "r"(d) : "memory");
    asm("sqc2 vf5, 0x10(%0)" : : "r"(d) : "memory");
    asm("sqc2 vf6, 0x20(%0)" : : "r"(d) : "memory");
    asm("sqc2 vf7, 0x30(%0)" : : "r"(d) : "memory");
}

void func_001C69A0(Model *model)
{
    int i;

    vu0_scale_matrix(D_70003400, D_70003400, &model->scale);

    for (i = 0; i < model->bone_count; i++) {
        Bone *bone = model->bone[i];
        float k;

        quat_nlerp(D_70003600, bone->qa, bone->qb, bone->t);
        quat_to_mat3((float *)D_70003440, D_70003600, bone);

        vu0_scale_xyz(&D_70003440[0], &D_70003440[0], bone->scale[0]);
        vu0_scale_xyz(&D_70003440[1], &D_70003440[1], bone->scale[1]);
        vu0_scale_xyz(&D_70003440[2], &D_70003440[2], bone->scale[2]);

        func_001029C0(D_70003480);
        func_00102C58(D_70003480, D_70003480, bone->rest_rot);
        D_70003480[3].x = bone->rest_trans[0];
        D_70003480[3].y = bone->rest_trans[1];
        D_70003480[3].z = bone->rest_trans[2];

        k = 1.0f / 4096.0f;
        vu0_scale_xyz(&D_70003480[0], &D_70003480[0], k * (float)bone->rest_scale[0]);
        vu0_scale_xyz(&D_70003480[1], &D_70003480[1], k * (float)bone->rest_scale[1]);
        vu0_scale_xyz(&D_70003480[2], &D_70003480[2], k * (float)bone->rest_scale[2]);

        vu0_mul_matrix(D_70003480, D_70003440, D_70003480);

        if (bone->parent != -1) {
            vu0_mul_matrix(bone->world, model->bone[bone->parent]->world, D_70003480);
        } else {
            vu0_mul_matrix(bone->world, D_70003400, D_70003480);
        }
    }
}

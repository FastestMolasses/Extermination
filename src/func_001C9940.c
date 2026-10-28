// NEARMISS func_001C9940  (vram 0x001C9940, 0x408 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 67.74% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (was 31.50%; matching wave 2026-10-28).
// Remaining differences: same blocker as func_001C9610: the original hands each row scale
// to VU0 through the integer register t0 (an integer load for the float scale fields, a
// float-to-integer move for the 4.12 scales); this candidate goes through a stack union.
// Needs: approval of a float-to-GPR move for the VU0 scalar operand.
// Inline VU0 asm (user-approved 2026-10-28 for the VU0 class), one instruction per statement.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Skeleton pose to world matrices (NLERP quaternion rotation, per-row scales, Euler basis,
// translation and 4.12 row scales, parented).

typedef struct { float x, y, z, w; } Vec4;
typedef struct { Vec4 r[4]; } Mtx4;

typedef struct Bone {
    float pivot[3];
    char  pad0C[0x0C];
    float rot_row_scale[3];
    char  pad24[0x0C];
    float quat_a[4];
    float quat_b[4];
    float blend;
    char  pad54[0x10];
    short parent;
    char  pad66[0x0A];
    float basis_euler[3];
    float translation[3];
    short row_scale_fp[3];
    char  pad8E[0x02];
    Mtx4  world;
} Bone;

extern Mtx4 D_70003400;
extern Mtx4 D_70003440;
extern Vec4 D_70003600;

extern void func_001029C0(Mtx4 *m);
extern void func_00102C58(Mtx4 *dst, Mtx4 *src, float *rot);
extern void quat_nlerp(Vec4 *out, float *a, float *b, float t);
extern void quat_to_mat3(Mtx4 *out, Vec4 *q, Bone *b);

static inline void vu0_scale_xyz(Vec4 *dst, Vec4 *src, float s)
{
    union { float f; int i; } u;

    u.f = s;
    asm("lqc2 vf4, 0x0(%0)" : : "r"(src));
    asm("qmtc2.ni %0, vf5" : : "r"(u.i));
    asm("vmulx.xyz vf4, vf4, vf5x");
    asm("sqc2 vf4, 0x0(%0)" : : "r"(dst));
}

static inline void vu0_mul_matrix(Mtx4 *m0, Mtx4 *m1, Mtx4 *m2)
{
    asm("lqc2 vf4, 0x0(%0)" : : "r"(m2));
    asm("lqc2 vf5, 0x10(%0)" : : "r"(m2));
    asm("lqc2 vf6, 0x20(%0)" : : "r"(m2));
    asm("lqc2 vf7, 0x30(%0)" : : "r"(m2));
    asm("lqc2 vf8, 0x0(%0)" : : "r"(m1));
    asm("vmulax.xyzw ACC, vf4, vf8x");
    asm("vmadday.xyzw ACC, vf5, vf8y");
    asm("vmaddaz.xyzw ACC, vf6, vf8z");
    asm("vmaddw.xyzw vf12, vf7, vf8w");
    asm("sqc2 vf12, 0x0(%0)" : : "r"(m0));
    asm("lqc2 vf9, 0x10(%0)" : : "r"(m1));
    asm("vmulax.xyzw ACC, vf4, vf9x");
    asm("vmadday.xyzw ACC, vf5, vf9y");
    asm("vmaddaz.xyzw ACC, vf6, vf9z");
    asm("vmaddw.xyzw vf13, vf7, vf9w");
    asm("sqc2 vf13, 0x10(%0)" : : "r"(m0));
    asm("lqc2 vf10, 0x20(%0)" : : "r"(m1));
    asm("vmulax.xyzw ACC, vf4, vf10x");
    asm("vmadday.xyzw ACC, vf5, vf10y");
    asm("vmaddaz.xyzw ACC, vf6, vf10z");
    asm("vmaddw.xyzw vf14, vf7, vf10w");
    asm("sqc2 vf14, 0x20(%0)" : : "r"(m0));
    asm("lqc2 vf11, 0x30(%0)" : : "r"(m1));
    asm("vmulax.xyzw ACC, vf4, vf11x");
    asm("vmadday.xyzw ACC, vf5, vf11y");
    asm("vmaddaz.xyzw ACC, vf6, vf11z");
    asm("vmaddw.xyzw vf15, vf7, vf11w");
    asm("sqc2 vf15, 0x30(%0)" : : "r"(m0));
}

void func_001C9940(Bone **bones, int n, Mtx4 *root)
{
    int i;

    for (i = 0; i < n; i++) {
        quat_nlerp(&D_70003600, bones[i]->quat_a, bones[i]->quat_b, bones[i]->blend);
        quat_to_mat3(&D_70003400, &D_70003600, bones[i]);
        vu0_scale_xyz(&D_70003400.r[0], &D_70003400.r[0], bones[i]->rot_row_scale[0]);
        vu0_scale_xyz(&D_70003400.r[1], &D_70003400.r[1], bones[i]->rot_row_scale[1]);
        vu0_scale_xyz(&D_70003400.r[2], &D_70003400.r[2], bones[i]->rot_row_scale[2]);
        func_001029C0(&D_70003440);
        func_00102C58(&D_70003440, &D_70003440, bones[i]->basis_euler);
        D_70003440.r[3].x = bones[i]->translation[0];
        D_70003440.r[3].y = bones[i]->translation[1];
        D_70003440.r[3].z = bones[i]->translation[2];
        vu0_scale_xyz(&D_70003440.r[0], &D_70003440.r[0],
                      (1.0f / 4096.0f) * (float)bones[i]->row_scale_fp[0]);
        vu0_scale_xyz(&D_70003440.r[1], &D_70003440.r[1],
                      (1.0f / 4096.0f) * (float)bones[i]->row_scale_fp[1]);
        vu0_scale_xyz(&D_70003440.r[2], &D_70003440.r[2],
                      (1.0f / 4096.0f) * (float)bones[i]->row_scale_fp[2]);
        vu0_mul_matrix(&D_70003440, &D_70003440, &D_70003400);
        if (bones[i]->parent != -1) {
            vu0_mul_matrix(&bones[i]->world, &D_70003440,
                           &bones[bones[i]->parent]->world);
        } else {
            vu0_mul_matrix(&bones[i]->world, &D_70003440, root);
        }
    }
}

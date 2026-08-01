// NEARMISS func_001C9940  (vram 0x001C9940, 0x408 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 31.50% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// compiler artifact (register coloring / scheduling)
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef struct Mat4 {
    float row[4][4];
} Mat4;

typedef struct Bone {
    float pivot[3];          /* 0x00 — vec3 handed to quat_to_mat3 as the translation row */
    char  pad0C[0x0C];       /* 0x0C */
    float rot_row_scale[3];  /* 0x18 — per-row scalars applied to the quat rotation */
    char  pad24[0x0C];       /* 0x24 */
    float quat_a[4];         /* 0x30 */
    float quat_b[4];         /* 0x40 */
    float blend;             /* 0x50 — NLERP t between quat_a and quat_b */
    char  pad54[0x10];       /* 0x54 */
    short parent;            /* 0x64 — index into bones[], -1 == attach to `root` */
    char  pad66[0x0A];       /* 0x66 */
    float basis_euler[3];    /* 0x70 — ZYX Euler angles (radians) for the local basis */
    float translation[3];    /* 0x7C */
    short row_scale_fp[3];   /* 0x88 — 4.12 fixed point, one per basis row */
    char  pad8E[0x02];       /* 0x8E */
    Mat4  world;             /* 0x90 — output */
} Bone;

/* EE scratchpad staging area (0x70000000 is the SPR). */
extern volatile float D_70003400[4];   /* R: rotation matrix, row 0 */
extern volatile float D_70003410[4];   /* R row 1 */
extern volatile float D_70003420[4];   /* R row 2 */
extern volatile float D_70003440[4];   /* L: local matrix, row 0 */
extern volatile float D_70003450[4];   /* L row 1 */
extern volatile float D_70003460[4];   /* L row 2 */
extern volatile float D_70003600[4];   /* blended quaternion */

extern void func_001029C0(void *m);                       /* 4x4 identity  (sceVu0UnitMatrix) */
extern void func_00102C58(void *dst, void *src, void *r); /* ZYX Euler rotate (sceVu0RotMatrix) */
extern void quat_nlerp(float *out, float *a, float *b, float t);
extern void quat_to_mat3(void *out, float *q, void *trans_src);

/* vmulx.xyz $vf4, $vf4, $vf5x — scale a row's xyz by a scalar, leave w alone.
   Emitted inline here; the out-of-line twin is func_00103230. */
static void vu0_scale_row_xyz(volatile float *row, float s)
{
    row[0] *= s;
    row[1] *= s;
    row[2] *= s;
}

/* vmulax/vmadday/vmaddaz/vmaddw accumulator chain, four rows.
   Row-vector convention: out.row[i] = sum_j lhs[i][j] * rhs.row[j].
   Emitted inline here; the out-of-line twin is func_001026D0(out, rhs, lhs). */
static void vu0_mul_matrix(volatile float *out, const volatile float *rhs,
                           const volatile float *lhs)
{
    int i, c;
    for (i = 0; i < 4; i++) {
        for (c = 0; c < 4; c++) {
            out[i * 4 + c] = lhs[i * 4 + 0] * rhs[0 * 4 + c]
                           + lhs[i * 4 + 1] * rhs[1 * 4 + c]
                           + lhs[i * 4 + 2] * rhs[2 * 4 + c]
                           + lhs[i * 4 + 3] * rhs[3 * 4 + c];
        }
    }
}

void func_001C9940(Bone **bones, int n, Mat4 *root)
{
    int i;

    for (i = 0; i < n; i++) {
        Bone *b = bones[i];

        /* 1. R (0x70003400) = rotation matrix of the NLERPed pose quat. */
        quat_nlerp((float *)D_70003600, b->quat_a, b->quat_b, b->blend);
        quat_to_mat3((void *)D_70003400, (float *)D_70003600, b);

        /* 2. Per-row float scale of R's basis. */
        vu0_scale_row_xyz(D_70003400, b->rot_row_scale[0]);
        vu0_scale_row_xyz(D_70003410, b->rot_row_scale[1]);
        vu0_scale_row_xyz(D_70003420, b->rot_row_scale[2]);

        /* 3. L (0x70003440) = Euler basis + translation. */
        func_001029C0((void *)D_70003440);
        func_00102C58((void *)D_70003440, (void *)D_70003440, b->basis_euler);
        *(volatile float *)0x70003470 = b->translation[0];
        *(volatile float *)0x70003474 = b->translation[1];
        *(volatile float *)0x70003478 = b->translation[2];

        /* 4. Per-row 4.12 fixed-point scale of L's basis. */
        vu0_scale_row_xyz(D_70003440, b->row_scale_fp[0] * (1.0f / 4096.0f));
        vu0_scale_row_xyz(D_70003450, b->row_scale_fp[1] * (1.0f / 4096.0f));
        vu0_scale_row_xyz(D_70003460, b->row_scale_fp[2] * (1.0f / 4096.0f));

        /* 5. L = L * R. */
        vu0_mul_matrix(D_70003440, D_70003400, D_70003440);

        /* 6. world = L * parent_world (or * root for a parentless bone). */
        if (b->parent != -1) {
            vu0_mul_matrix((volatile float *)&b->world,
                           (const volatile float *)&bones[b->parent]->world,
                           D_70003440);
        } else {
            vu0_mul_matrix((volatile float *)&b->world,
                           (const volatile float *)root,
                           D_70003440);
        }
    }
}

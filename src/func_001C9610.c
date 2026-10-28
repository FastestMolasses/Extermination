// NEARMISS func_001C9610  (vram 0x001C9610, 0x330 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 69.18% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (was 32.66%; matching wave 2026-10-28).
// Remaining differences: the original moves each row scale from a float register into
// the integer register t0 (a float-to-integer move, not a VU0 instruction) right before the
// VU0 scalar load; mwcc cannot express that in C (a float "r" operand is silently left
// uncomputed, a pointer pun goes through memory), so this candidate passes the scale
// through a union on the stack. With a (non-approved) asm move the score is still only
// 72.35: the inlined helpers' arguments are also materialised differently (the original
// keeps the destination and source addresses in separate argument registers).
// Needs: approval of a float-to-GPR move for the VU0 scalar operand.
// Inline VU0 asm (user-approved 2026-10-28 for the VU0 class), one instruction per statement.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Skeleton pose to world matrices (scale + Euler + translation + bind, parented).

typedef struct { float x, y, z, w; } Vec4;
typedef struct { Vec4 r[4]; } Mtx4;

typedef struct SkelNode {
    /* 0x00 */ Mtx4  bind;
    /* 0x40 */ char  pad40[0x64 - 0x40];
    /* 0x64 */ short parent;
    /* 0x66 */ char  pad66[0x70 - 0x66];
    /* 0x70 */ float rot[3];
    /* 0x7C */ float trans[3];
    /* 0x88 */ short scale[3];
    /* 0x8E */ char  pad8E[0x90 - 0x8E];
    /* 0x90 */ Mtx4  world;
} SkelNode;

extern Mtx4 D_70003400;

extern void func_001029C0(Mtx4 *m);
extern void func_00102C58(Mtx4 *dst, Mtx4 *src, float *rot);

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

void func_001C9610(SkelNode **nodes, int count, Mtx4 *root)
{
    int i;

    for (i = 0; i < count; i++) {
        func_001029C0(&D_70003400);
        func_00102C58(&D_70003400, &D_70003400, nodes[i]->rot);
        D_70003400.r[3].x = nodes[i]->trans[0];
        D_70003400.r[3].y = nodes[i]->trans[1];
        D_70003400.r[3].z = nodes[i]->trans[2];
        vu0_scale_xyz(&D_70003400.r[0], &D_70003400.r[0],
                      (1.0f / 4096.0f) * (float)nodes[i]->scale[0]);
        vu0_scale_xyz(&D_70003400.r[1], &D_70003400.r[1],
                      (1.0f / 4096.0f) * (float)nodes[i]->scale[1]);
        vu0_scale_xyz(&D_70003400.r[2], &D_70003400.r[2],
                      (1.0f / 4096.0f) * (float)nodes[i]->scale[2]);
        vu0_mul_matrix(&D_70003400, &D_70003400, &nodes[i]->bind);
        if (nodes[i]->parent != -1) {
            vu0_mul_matrix(&nodes[i]->world, &D_70003400,
                           &nodes[nodes[i]->parent]->world);
        } else {
            vu0_mul_matrix(&nodes[i]->world, &D_70003400, root);
        }
    }
}

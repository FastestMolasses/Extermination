// CFLAGS: -O4,p -sdatathreshold 4
/* exterm_prelude.h — inlined ahead of m2c --valid-syntax output so it compiles
 * under CodeWarrior mwccmips (the project uses no system headers). Base int/float
 * widths first, then the m2c_macros.h definitions. Local tooling only — never
 * shipped, never part of a matched src file (matched C uses plain types, no header). */
#ifndef EXTERM_M2C_PRELUDE_H
#define EXTERM_M2C_PRELUDE_H

typedef signed char        s8;
typedef unsigned char      u8;
typedef short              s16;
typedef unsigned short     u16;
typedef int                s32;
typedef unsigned int       u32;
typedef long long          s64;
typedef unsigned long long u64;
typedef float              f32;
typedef double             f64;
/* PERMUTER preprocesses base.c with `cpp -DPERMUTER` then parses it with pycparser,
 * which can't handle GCC mode() attributes — give it a parseable struct form there.
 * The real mwcc compile (no -DPERMUTER) gets the true 128-bit type. */
#ifdef PERMUTER
typedef struct { int _q0, _q1, _q2, _q3; } s128;
typedef struct { unsigned _q0, _q1, _q2, _q3; } u128;
#else
typedef int   s128 __attribute__((mode(TI)));
typedef unsigned u128 __attribute__((mode(TI)));
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

extern void *memcpy(void *, const void *, unsigned int);

/* ---- from m2c_macros.h (valid-syntax mode) ---- */
typedef s32 M2C_UNK;
typedef s8  M2C_UNK8;
typedef s16 M2C_UNK16;
typedef s32 M2C_UNK32;
typedef s64 M2C_UNK64;

#define M2C_FIELD(expr, type_ptr, offset) (*(type_ptr)((s8 *)(expr) + (offset)))
#define M2C_BITWISE(type, expr) ((type)(expr))
#define M2C_LWL(expr) (expr)
#define M2C_FIRST3BYTES(expr) (expr)
#define M2C_UNALIGNED32(expr) (expr)
#define M2C_ERROR(desc) (0)
#define M2C_TRAP_IF(cond) (0)
#define M2C_BREAK() (0)
#define M2C_SYNC() (0)
#define GLUE_F64(a, b) (0.0)
#define MULT_HI(a, b) (0)
#define MULTU_HI(a, b) (0)
#define DMULT_HI(a, b) (0)
#define DMULTU_HI(a, b) (0)
#define CLZ(x) (0)
#define REVERSE_BITS(x) (0)
#define ROTATE_RIGHT(x, shift) (0)
#define ARM_RRX(x, carry) (0)
#define BSWAP32(x) (0)
#define BSWAP16(x) (0)
#define BSWAP16X2(x) (0)
#define M2C_CARRY 0
#define M2C_OVERFLOW(a) (0)
#define M2C_MEMCPY_ALIGNED memcpy
#define M2C_MEMCPY_UNALIGNED memcpy
#define M2C_STRUCT_COPY memcpy

#endif

extern s8 *D_00275B40;
struct S { char _pad[0x10]; float f10; float f14; };
s32 func_001BD180(struct S *arg0) {
    float k = 0.2f;
    float temp_f1;
    float temp_f1_2;
    temp_f1 = arg0->f10 + k;
    arg0->f10 = temp_f1;
    if (!(temp_f1 < 6.0f)) {
        if (!(temp_f1 < 14.0f)) { arg0->f10 = 14.0f; }
        temp_f1_2 = arg0->f14 + k;
        arg0->f14 = temp_f1_2;
        if (!(temp_f1_2 <= 16.0f)) { arg0->f14 = 16.0f; return 1; }
        goto block_5;
    }
block_5:
    M2C_FIELD(M2C_FIELD(D_00275B40, s8 **, 4), f32 *, 0x80) = (f32) (arg0->f10 + arg0->f14);
    M2C_FIELD(M2C_FIELD(D_00275B40, s8 **, 8), f32 *, 0x80) = (f32) (arg0->f10 + arg0->f14);
    M2C_FIELD(M2C_FIELD(D_00275B40, s8 **, 0xC), f32 *, 0x80) = (f32) arg0->f14;
    M2C_FIELD(M2C_FIELD(D_00275B40, s8 **, 0x10), f32 *, 0x80) = (f32) arg0->f14;
    return 0;
}

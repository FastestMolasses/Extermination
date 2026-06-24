// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
//
// Builds a GIF/GS DMA packet into the per-channel buffer selected by arg0.
// t3 = D_00275670 + arg0*4 indexes a channel record whose +0x10 word is the
// current write pointer (reloaded after each header store -- aliasing reload
// idiom). Writes a small header (byte 0x10 at +3, clears +4, 0x0005 at +0),
// then advances the write pointer by 0x60 and fills the 0x60-byte packet at q:
//   +0x10: zeroed quadword (single sq)
//   +0x1C: 0x50000004 ; +0x20: 0x8001 | (0x54000000<<32)
//   +0x28: 0x44410   ; +0x30: 0x143   ; +0x38: low 32 bits of arg4 (zero-ext)
// then three 64-bit packed XYZ words at q+0x40/+0x48/+0x50, each formed from
// two float_to_int conversions (arg1/arg2/arg3 .x at +0, .z at +8) as
// (int)(z_i | (x_i<<16)) sign-extended to 64 | (0xFFFFFF<<32); +0x58 zeroed.
// Local declaration order (p,q before the constants c5/c16) is load-bearing:
// it pins the reloaded write pointer to t0 and the header constants to t1/t2,
// matching CodeWarrior's caller-saved temp allocation.
typedef int u128 __attribute__((mode(TI)));
extern int float_to_int(float);
extern char *D_00275670;

void func_00208040(int arg0, float *arg1, float *arg2, float *arg3, unsigned long long arg4) {
    char *p;
    char *q;
    int c5 = 5;
    int c16 = 0x10;
    char *t3 = D_00275670 + (arg0 << 2);

    p = *(char **)(t3 + 0x10);
    *(char *)(p + 3) = c16;
    p = *(char **)(t3 + 0x10);
    *(int *)(p + 4) = 0;
    p = *(char **)(t3 + 0x10);
    *(short *)(p + 0) = c5;
    q = *(char **)(t3 + 0x10);
    *(char **)(t3 + 0x10) = q + 0x60;

    ((u128 *)(q + 0x10))[0] = 0;
    *(int *)(q + 0x1C) = 0x50000004;
    *(long long *)(q + 0x20) = (long long)0x8001 | ((long long)0x54000000 << 32);
    *(long long *)(q + 0x28) = (long long)0x44410;
    *(long long *)(q + 0x30) = (long long)0x143;
    *(long long *)(q + 0x38) = (long long)(arg4 << 32 >> 32);

    {
        int fa, fb;
        char *r = q + 0x10;
        fa = float_to_int(arg1[2]) << 16;
        fb = float_to_int(arg1[0]);
        *(long long *)(r + 0x30) = (long long)(int)(fb | fa) | ((long long)0xFFFFFF << 32);
        fa = float_to_int(arg2[2]) << 16;
        fb = float_to_int(arg2[0]);
        *(long long *)(r + 0x38) = (long long)(int)(fb | fa) | ((long long)0xFFFFFF << 32);
        fa = float_to_int(arg3[2]) << 16;
        fb = float_to_int(arg3[0]);
        *(long long *)(r + 0x40) = (long long)(int)(fb | fa) | ((long long)0xFFFFFF << 32);
        *(long long *)(r + 0x48) = 0;
    }
}

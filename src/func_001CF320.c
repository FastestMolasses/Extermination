// CFLAGS: -O4,p -sdatathreshold 0
typedef signed char        s8;
typedef unsigned char      u8;
typedef int                s32;
typedef long long          s64;
typedef int   s128 __attribute__((mode(TI)));

s32 float_to_int(float);
s8 *func_001CB5F0(void *, int, int);
extern s8 D_007635C0;

void func_001CF320(s8 *arg0, s8 *arg1, s64 arg2, float fparg0) {
    s32 temp_s0;
    s32 temp_s1;
    s8 *v;

    temp_s0 = float_to_int(48.0f * fparg0) * 0x10;
    temp_s1 = float_to_int(24.0f * fparg0) * 0x10;
    {int eight = 8; v = func_001CB5F0(&D_007635C0, 0xFFF000, eight);}
    *(s128 *)(v + 0) = 0;
    *(s32 *)(v + 0xC) = 0x50000007;
    *(s64 *)(v + 0x10) = ((s64)0x602B4000 << 32) | 0x8001;
    *(s64 *)(v + 0x18) = 0x424216;
    *(s64 *)(v + 0x20) = arg2;
    *(s32 *)(v + 0x30) = *(s32 *)(arg1 + 0);
    *(s32 *)(v + 0x34) = *(s32 *)(arg1 + 4);
    *(s32 *)(v + 0x38) = *(s32 *)(arg1 + 8);
    *(s32 *)(v + 0x3C) = *(s32 *)(arg1 + 0xC);
    *(s32 *)(v + 0x40) = 0;
    *(s32 *)(v + 0x44) = 0;
    *(s32 *)(v + 0x48) = 0x3F800000;
    *(s32 *)(v + 0x50) = *(s32 *)(arg0 + 0) + temp_s0;
    *(s32 *)(v + 0x54) = *(s32 *)(arg0 + 4) + temp_s1;
    *(s32 *)(v + 0x58) = 0xFFFFFF;
    *(s32 *)(v + 0x5C) = 0;
    *(s32 *)(v + 0x60) = 0x3F800000;
    *(s32 *)(v + 0x64) = 0x3F800000;
    *(s32 *)(v + 0x68) = 0x3F800000;
    *(s32 *)(v + 0x70) = *(s32 *)(arg0 + 0) - temp_s0;
    *(s32 *)(v + 0x74) = *(s32 *)(arg0 + 4) - temp_s1;
    *(s32 *)(v + 0x78) = 0xFFFFFF;
    *(s32 *)(v + 0x7C) = 0;
}

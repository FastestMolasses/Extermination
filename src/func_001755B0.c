// CFLAGS: -O4,p -sdatathreshold 0
typedef signed char s8;
typedef int s32;
typedef float f32;

f32 func_0011DF78(f32);
f32 func_001B1470(f32);
extern f32 D_008106A0;

s32 func_001755B0(s8 *arg0) {
    f32 temp_f0;

    temp_f0 = func_0011DF78(func_001B1470(func_001B1470(3.1415927f + *(f32 *)(arg0 + 0x24C) + D_008106A0) - *(f32 *)(arg0 + 0xC4)));
    *(f32 *)0x70003A20 = temp_f0;
    if (temp_f0 > 1.5707964f) {
        return 1;
    }
    return 0;
}

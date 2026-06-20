// CFLAGS: -O4,p -sdatathreshold 0
typedef signed char s8; typedef unsigned char u8; typedef unsigned short u16; typedef int s32; typedef float f32;
extern f32 func_0011DF78(f32);
extern f32 func_001B1470(f32);
extern f32 D_0081069C;
extern u16 D_00810E74;
s32 func_00191000(s8 *a0, s8 *a1) {
    if (D_00810E74 & *(u16*)0x70003B80) {
        if (*(u8*)(a1+0x1F0) == 6) {
            *(f32*)(a0+0x48) = func_001B1470(3.1415927f + *(f32*)(a1+0xC4));
        } else {
            *(f32*)(a0+0x48) = *(f32*)(a1+0xC4);
        }
        if (func_0011DF78(func_001B1470(*(f32*)(a0+0x48) - *(f32*)(a0+0x44))) > 0.05235988f) {
            *(s8*)(a0+6) = 3;
            *(s8*)(a0+1) = 0;
            *(f32*)(a0+0x4C) = func_0011DF78(D_0081069C);
            if (*(f32*)(a0+0x4C) < 7.0f) {
                *(f32*)(a0+0x4C) = 7.0f;
            } else if (!(*(f32*)(a0+0x4C) <= func_0011DF78(*(f32*)(a0+0x64)))) {
                *(f32*)(a0+0x4C) = func_0011DF78(*(f32*)(a0+0x64));
            }
            return 1;
        }
    }
    return 0;
}
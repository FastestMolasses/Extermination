// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Matched via mwcc 2.3.3 (idiom-13-family delay-slot/dead-const wall that 991202 fills).
typedef signed char        s8;
typedef unsigned char      u8;
typedef short              s16;
typedef unsigned short     u16;
typedef int                s32;
typedef unsigned int       u32;
typedef float              f32;

extern u8 D_00810700;

void func_00161690(s8 *arg0) {
    f32 temp_f1;
    f32 temp_f1_2;
    f32 temp_f1_3;

    if (D_00810700 == 0xB) {
        temp_f1 = *(f32 *)(arg0 + 0xB4) + *(f32 *)(arg0 + 0x254);
        if (!(temp_f1 < 285.0f) && (temp_f1 <= 587.0f)) {
            temp_f1_2 = *(f32 *)(arg0 + 0x2E0);
            if (!(temp_f1_2 < 400.0f) && (temp_f1_2 <= 410.0f)) {
                temp_f1_3 = *(f32 *)(arg0 + 0x2E8);
                if (!(temp_f1_3 < 236.0f) && (temp_f1_3 <= 246.0f)) {
                    *(s32 *)(arg0 + 0x2E0) = 0x43CA8000;
                    *(s32 *)(arg0 + 0x2E8) = 0x43710000;
                    *(s32 *)(arg0 + 0xC4) = 0x3FBBA866;
                }
            }
        }
    }
}

// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
//
// Randomises a spawn position from func_00122BB8 (rand) bytes. When the
// ctl+0x65 variant byte is not 2: self+0xB8 = 423 * (byte / 255) - 270, a
// scratch offset at 0x70003A20 = 150 * (byte / 255), and self+0xB0 =
// 250 + offset when the variant is nonzero, else -200 + offset. Variant 2:
// self+0xB0 = -245 + 690 * (byte / 255) and self+0xB8 = -270 + 200 * (byte /
// 255). Each byte is a different bit slice of a fresh rand() (>>13, >>19,
// >>14, >>17). The scratchpad word is a plain relocated extern (idiom-32).
extern int func_00122BB8(void);
extern float D_70003A20;

void func_00150DA0(char *self, char *ctl) {
    if (*(signed char *)(ctl + 0x65) != 2) {
        *(float *)(self + 0xB8) =
            423.0f * ((float)((func_00122BB8() >> 13) & 0xFF) / 255.0f) - 270.0f;
        D_70003A20 =
            150.0f * ((float)((func_00122BB8() >> 19) & 0xFF) / 255.0f);
        if (*(signed char *)(ctl + 0x65) != 0) {
            *(float *)(self + 0xB0) = 250.0f + D_70003A20;
        } else {
            *(float *)(self + 0xB0) = -200.0f + D_70003A20;
        }
    } else {
        *(float *)(self + 0xB0) =
            -245.0f + 690.0f * ((float)((func_00122BB8() >> 14) & 0xFF) / 255.0f);
        *(float *)(self + 0xB8) =
            -270.0f + 200.0f * ((float)((func_00122BB8() >> 17) & 0xFF) / 255.0f);
    }
}

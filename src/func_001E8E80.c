// NEARMISS func_001E8E80  (vram 0x001E8E80, 0x3F4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.05% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation/addressing-derivation permutation (idiom exhaustion reached). The mula.s/madd.s FMA in the target (dx*dx+dy*dy via func_0011E748) is NOT the FPU-MAC wall -- writing it as plain (sx-3)*(sx-3)+(bx-3)*(bx-3) reproduces the exact FMA under mwcc233 once the statement order matches ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern float func_0011E748(float d);
extern int func_00122BB8(void);
extern char *D_00275C18;

// Initializes an 8x8 grid (64 elements) of particle/spark records at
// D_00275C18 + bone_idx*0xA060 for a tendril-mesh effect. tuning constants
// at +0x00..0x50 are seeded once, then each grid cell's position (+0x60/64/68),
// scale (+0x6C), and normalized grid coords (+0x4060/4064) are computed from a
// quadratic/random blend of the base position (arg0+0xB0/B4/B8) and the
// spread vector (arg2+0x0/8). +0x8060/-0x6FA0(alias) flag words are cleared.
void func_001E8E80(char *arg0, int idx, char *arg2) {
    char *base = D_00275C18 + idx * 0xA060;
    int bx;
    int sx;
    char *cell;
    char *flag;

    *(int *)(base + 0x40) = 0x3F333333;
    *(int *)(base + 0x44) = 0x3EE3D70A;
    *(int *)(base + 0x48) = 0x437F0000;
    *(int *)(base + 0x4C) = 0;
    *(int *)(base + 0x50) = 0xBFA66666;
    *(float *)(base + 0) = *(float *)(arg0 + 0xB0);
    *(float *)(base + 4) = *(float *)(arg0 + 0xB4);
    *(float *)(base + 8) = *(float *)(arg0 + 0xB8);
    *(float *)(base + 0x30) = *(float *)(arg2 + 0);
    *(float *)(base + 0x34) = *(float *)(arg2 + 8);
    *(int *)(base + 0x3C) = 0x3EA66666;
    *(int *)(base + 0x38) = 0x3EA66666;
    *(int *)(base + 0x10) = 0x42A60000;
    *(int *)(base + 0x14) = 0x42A60000;
    *(int *)(base + 0x18) = 0x42BE0000;
    *(int *)(base + 0x1C) = 0;
    *(int *)(base + 0x20) = 0x3F7DB22D;
    *(int *)(base + 0x24) = 0x3CB43958;
    *(int *)(base + 0x2C) = 0x3ECCCCCD;
    *(int *)(base + 0x28) = 0x3D4CCCCD;
    *(int *)(base + 0x38) = 0x3F59999A;
    *(int *)(base + 0x3C) = 0x3F51EB85;

    for (bx = 0; bx < 8; bx++) {
        cell = base + bx * 0x200;
        flag = base + bx * 0x80;
        for (sx = 0; sx < 8; sx++) {
            char *f = flag + sx * 4;
            char *c = cell + sx * 0x10;
            float dist;
            float scale;

            *(float *)(c + 0x60) = (*(float *)(base + 0) - 0.5f * *(float *)(base + 0x30)) + (*(float *)(base + 0x30) * (float)sx) / 6.0f;
            *(float *)(c + 0x68) = (*(float *)(base + 8) - 0.5f * *(float *)(base + 0x34)) + (*(float *)(base + 0x34) * (float)bx) / 6.0f;
            *(float *)(c + 0x64) = *(float *)(base + 4);

            *(float *)(c + 0x60) = *(float *)(c + 0x60) + (0.1f * ((float)func_00122BB8() / 2147483648.0f) - 0.05f);
            *(float *)(c + 0x68) = *(float *)(c + 0x68) + (0.1f * ((float)func_00122BB8() / 2147483648.0f) - 0.05f);

            dist = (3.0f - func_0011E748((float)(sx - 3) * (float)(sx - 3) + (float)(bx - 3) * (float)(bx - 3))) / 4.0f;
            if (dist > 0.1f) {
                *(float *)(c + 0x64) = *(float *)(c + 0x64) + 0.15f;
                *(float *)(c + 0x64) = *(float *)(c + 0x64) + (0.05f * (float)func_00122BB8()) / 2147483648.0f;
            }
            if (dist > 0.35f) {
                scale = 1.0f;
            } else {
                scale = dist * 2.0f;
            }
            if (scale < 0.0f) {
                scale = 0.0f;
            }
            *(float *)(c + 0x6C) = 128.0f * scale;
            *(float *)(c + 0x4060) = (float)sx / 8.0f;
            *(float *)(c + 0x4064) = (float)bx / 8.0f;
            *(int *)(f + 0x8060) = 0;
            *(int *)(f + 0x9060) = 0;
        }
    }

    arg0[0xC] = 0;
    arg0[9] = 0;
    arg0[4] = 1;
}

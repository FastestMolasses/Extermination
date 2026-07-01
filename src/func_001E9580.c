// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Tendril-mesh record initializer, sibling of the committed renderer
// func_001E9E60. Initializes the D_00275C1C + arg1*0xA060 record: default
// physics/palette params, origin from arg0+0xB0/B4/B8, cell spacing from
// arg2[0]/arg2[2]; then a per-area switch on key = (D_00810700<<8)|D_00810701
// overriding palette (+0x10/14/18 RGB), params (+0x40/48/4C/50), style byte
// (+0x5C) and the +0x38/+0x3C pair; finally an 8x8 lattice loop laying out
// vertices at +0x60 (stride 0x10, row 0x200), a dome height profile
// t = clamp01ish((3 - sqrtf((i-3)^2+(j-3)^2)) / 4) -> +0x6C (t>0.15 saturates
// to 1.0, else doubled, negative clamped to 0) with randomized Y bumps
// (+0.15 + 0.05*rand()/2^31) applied to +0x64 inside the dome interior
// (t > 0.1), UVs (i/8, j/8) at +0x4060/+0x4064, and zeroing the two int
// lattices at +0x8060/+0x9060 (int index r[0x2018]/r[0x2418], stride 4,
// row 0x80). func_0011E748 = sqrtf, func_00122BB8 = rand.
extern float func_0011E748(float);
extern int func_00122BB8(void);

extern char *D_00275C1C;
extern unsigned char D_00810700[64];
extern unsigned char D_00810701[64];

void func_001E9580(char *arg0, int arg1, float *arg2) {
    char *p;
    int i;
    int j;
    char *q;
    int *r;
    char *row;
    int *rowb;
    int key;
    float t;
    float dx;
    float dz;

    p = D_00275C1C + arg1 * 0xA060;
    *(float *)(p + 0x40) = 0.005f;
    *(float *)(p + 0x44) = 0.445f;
    *(float *)(p + 0x48) = 50.0f;
    *(float *)(p + 0x4C) = -9.0f;
    *(float *)(p + 0x50) = -1.7f;
    *(float *)(p + 0x0) = *(float *)(arg0 + 0xB0);
    *(float *)(p + 0x4) = *(float *)(arg0 + 0xB4);
    *(float *)(p + 0x8) = *(float *)(arg0 + 0xB8);
    *(float *)(p + 0x30) = arg2[0];
    *(float *)(p + 0x34) = arg2[2];

    key = (D_00810700[0] << 8) + D_00810701[0];
    switch (key) {
    case 0x1:
        *(unsigned char *)(p + 0x5C) = 1;
        *(float *)(p + 0x48) = 12.8f;
        *(float *)(p + 0x4C) = 48.0f;
        *(float *)(p + 0x50) = -1.5f;
        *(float *)(p + 0x40) = 0.014f;
        *(float *)(p + 0x10) = 128.0f;
        *(float *)(p + 0x14) = 102.0f;
        *(float *)(p + 0x18) = 122.0f;
        *(float *)(p + 0x1C) = 0.0f;
        *(float *)(p + 0x3C) = 0.21f;
        *(float *)(p + 0x38) = 0.25f;
        break;
    case 0x1001:
        *(unsigned char *)(p + 0x5C) = 0;
        *(float *)(p + 0x48) = 12.8f;
        *(float *)(p + 0x4C) = 33.0f;
        *(float *)(p + 0x50) = -1.5f;
        *(float *)(p + 0x40) = 0.014f;
        *(float *)(p + 0x10) = 128.0f;
        *(float *)(p + 0x14) = 102.0f;
        *(float *)(p + 0x18) = 122.0f;
        *(float *)(p + 0x1C) = 0.0f;
        *(float *)(p + 0x3C) = 0.21f;
        *(float *)(p + 0x38) = 0.25f;
        break;
    case 0x0:
    case 0x200:
        *(unsigned char *)(p + 0x5C) = 0;
        *(float *)(p + 0x48) = 22.8f;
        *(float *)(p + 0x4C) = 49.0f;
        *(float *)(p + 0x50) = -1.5f;
        *(float *)(p + 0x40) = 0.014f;
        *(float *)(p + 0x10) = 128.0f;
        *(float *)(p + 0x14) = 128.0f;
        *(float *)(p + 0x18) = 128.0f;
        *(float *)(p + 0x1C) = 0.0f;
        *(float *)(p + 0x3C) = 0.265f;
        *(float *)(p + 0x38) = 0.265f;
        break;
    case 0x2:
    case 0x101:
    case 0x601:
        *(unsigned char *)(p + 0x5C) = 0;
        *(float *)(p + 0x48) = 12.8f;
        *(float *)(p + 0x4C) = 48.0f;
        *(float *)(p + 0x50) = -1.5f;
        *(float *)(p + 0x40) = 0.014f;
        *(float *)(p + 0x10) = 128.0f;
        *(float *)(p + 0x14) = 102.0f;
        *(float *)(p + 0x18) = 122.0f;
        *(float *)(p + 0x1C) = 0.0f;
        *(float *)(p + 0x3C) = 0.265f;
        *(float *)(p + 0x38) = 0.265f;
        break;
    case 0x401:
        *(unsigned char *)(p + 0x5C) = 0;
        *(float *)(p + 0x48) = 12.8f;
        *(float *)(p + 0x4C) = 48.0f;
        *(float *)(p + 0x50) = -1.5f;
        *(float *)(p + 0x40) = 0.014f;
        *(float *)(p + 0x10) = 128.0f;
        *(float *)(p + 0x14) = 88.0f;
        *(float *)(p + 0x18) = 88.0f;
        *(float *)(p + 0x1C) = 0.0f;
        *(float *)(p + 0x3C) = 0.265f;
        *(float *)(p + 0x38) = 0.265f;
        break;
    case 0x202:
        *(unsigned char *)(p + 0x5C) = 0;
        *(float *)(p + 0x48) = 12.8f;
        *(float *)(p + 0x4C) = 70.0f;
        *(float *)(p + 0x50) = -1.5f;
        *(float *)(p + 0x40) = 0.014f;
        *(float *)(p + 0x10) = 128.0f;
        *(float *)(p + 0x14) = 102.0f;
        *(float *)(p + 0x18) = 122.0f;
        *(float *)(p + 0x1C) = 0.0f;
        *(float *)(p + 0x3C) = 0.265f;
        *(float *)(p + 0x38) = 0.265f;
        break;
    case 0x600:
        *(unsigned char *)(p + 0x5C) = 1;
        *(float *)(p + 0x48) = 12.8f;
        *(float *)(p + 0x4C) = 70.0f;
        *(float *)(p + 0x50) = -1.5f;
        *(float *)(p + 0x40) = 0.014f;
        *(float *)(p + 0x10) = 128.0f;
        *(float *)(p + 0x14) = 102.0f;
        *(float *)(p + 0x18) = 122.0f;
        *(float *)(p + 0x1C) = 0.0f;
        *(float *)(p + 0x3C) = 0.265f;
        *(float *)(p + 0x38) = 0.265f;
        break;
    case 0x300:
    case 0x400:
    case 0x1000:
    case 0x1301:
    case 0x1400:
    default:
        *(unsigned char *)(p + 0x5C) = 1;
        *(float *)(p + 0x48) = 12.8f;
        *(float *)(p + 0x4C) = 48.0f;
        *(float *)(p + 0x50) = -1.5f;
        *(float *)(p + 0x40) = 0.014f;
        *(float *)(p + 0x10) = 128.0f;
        *(float *)(p + 0x14) = 110.0f;
        *(float *)(p + 0x18) = 128.0f;
        *(float *)(p + 0x1C) = 0.0f;
        *(float *)(p + 0x3C) = 0.265f;
        *(float *)(p + 0x38) = 0.265f;
        break;
    case 0x100:
    case 0xD00:
        *(unsigned char *)(p + 0x5C) = 1;
        *(float *)(p + 0x48) = 12.8f;
        *(float *)(p + 0x4C) = 70.0f;
        *(float *)(p + 0x50) = -1.5f;
        *(float *)(p + 0x40) = 0.014f;
        *(float *)(p + 0x10) = 128.0f;
        *(float *)(p + 0x14) = 110.0f;
        *(float *)(p + 0x18) = 128.0f;
        *(float *)(p + 0x1C) = 0.0f;
        *(float *)(p + 0x3C) = 0.265f;
        *(float *)(p + 0x38) = 0.265f;
        break;
    case 0x700:
        *(unsigned char *)(p + 0x5C) = 1;
        *(float *)(p + 0x48) = 12.8f;
        *(float *)(p + 0x4C) = 48.0f;
        *(float *)(p + 0x50) = -1.5f;
        *(float *)(p + 0x40) = 0.014f;
        *(float *)(p + 0x10) = 125.0f;
        *(float *)(p + 0x14) = 96.0f;
        *(float *)(p + 0x18) = 128.0f;
        *(float *)(p + 0x1C) = 0.0f;
        *(float *)(p + 0x3C) = 0.265f;
        *(float *)(p + 0x38) = 0.265f;
        break;
    case 0x702:
    case 0x703:
        *(unsigned char *)(p + 0x5C) = 1;
        *(float *)(p + 0x48) = 12.8f;
        *(float *)(p + 0x4C) = 48.0f;
        *(float *)(p + 0x50) = -1.5f;
        *(float *)(p + 0x40) = 0.014f;
        *(float *)(p + 0x10) = 127.0f;
        *(float *)(p + 0x14) = 104.0f;
        *(float *)(p + 0x18) = 128.0f;
        *(float *)(p + 0x1C) = 0.0f;
        *(float *)(p + 0x3C) = 0.265f;
        *(float *)(p + 0x38) = 0.265f;
        break;
    case 0x803:
        *(unsigned char *)(p + 0x5C) = 0;
        *(float *)(p + 0x48) = 12.8f;
        *(float *)(p + 0x4C) = 48.0f;
        *(float *)(p + 0x50) = -1.5f;
        *(float *)(p + 0x40) = 0.014f;
        *(float *)(p + 0x10) = 97.0f;
        *(float *)(p + 0x14) = 95.0f;
        *(float *)(p + 0x18) = 128.0f;
        *(float *)(p + 0x1C) = 0.0f;
        *(float *)(p + 0x3C) = 0.265f;
        *(float *)(p + 0x38) = 0.265f;
        break;
    case 0x1300:
        *(unsigned char *)(p + 0x5C) = 1;
        *(float *)(p + 0x48) = 12.8f;
        *(float *)(p + 0x4C) = 78.0f;
        *(float *)(p + 0x50) = -1.5f;
        *(float *)(p + 0x40) = 0.014f;
        *(float *)(p + 0x10) = 95.0f;
        *(float *)(p + 0x14) = 80.0f;
        *(float *)(p + 0x18) = 128.0f;
        *(float *)(p + 0x1C) = 0.0f;
        *(float *)(p + 0x3C) = 0.265f;
        *(float *)(p + 0x38) = 0.265f;
        break;
    }

    *(float *)(p + 0x20) = 0.991f;
    *(float *)(p + 0x24) = 0.022f;
    *(float *)(p + 0x2C) = 0.4f;
    *(float *)(p + 0x28) = 0.05f;

    for (j = 0, row = p, rowb = (int *)p; j < 8; j++, row += 0x200, rowb += 0x20) {
        for (i = 0, q = row, r = rowb; i < 8; i++, q += 0x10, r += 1) {
            *(float *)(q + 0x60) = (*(float *)(p + 0x0) - 0.5f * *(float *)(p + 0x30)) +
                                   (*(float *)(p + 0x30) * (float)i) / 6.0f;
            *(float *)(q + 0x68) = (*(float *)(p + 0x8) - 0.5f * *(float *)(p + 0x34)) +
                                   (*(float *)(p + 0x34) * (float)j) / 6.0f;
            *(float *)(q + 0x64) = *(float *)(p + 0x4);
            dx = (float)(i - 3);
            dz = (float)(j - 3);
            t = 3.0f - func_0011E748(dx * dx + dz * dz);
            t = t / 4.0f;
            if (t > 0.1f) {
                *(float *)(q + 0x64) += 0.15f;
                *(float *)(q + 0x64) += (0.05f * (float)func_00122BB8()) / 2147483648.0f;
            }
            if (t > 0.15f) {
                t = 1.0f;
            } else {
                t *= 2.0f;
            }
            if (t < 0.0f) {
                t = 0.0f;
            }
            *(float *)(q + 0x6C) = t;
            *(float *)(q + 0x4060) = (float)i / 8.0f;
            *(float *)(q + 0x4064) = (float)j / 8.0f;
            r[0x2018] = 0;
            r[0x2418] = 0;
        }
    }
}

// NEARMISS func_001E13E0  (vram 0x001E13E0, 0x37C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 81.71% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation/frame-spill permutation. Both builds fully recover the logic (channel-table indexed by arg0*4 off the gp-rel base D_00275670, per-row GIFtag header build, 16-vertex inner loop, peeled 17th wrap-record) with byte-identical instruction CONTENT throughout -- the diff shows only r...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Builds a per-frame GIFtag sprite-grid packet for the 16x16 water surface
// table at D_0081E0F0. Indexes the channel table at D_00275670+arg0*4; the
// entry's +0x10 field is a bump-allocator cursor pointer advanced by 0x690
// per row. Each row emits a GIFtag header (REGLIST 0x600A4000.../0x512512)
// then 16 vertex pairs (rows i and i+1) of 0x60 bytes plus a peeled wrap
// record. float_to_int converts the 30720 + 16*coord screen positions; row
// 15 adds a 1.0/0x1000 bias.
extern char *D_00275670;
extern unsigned char D_0081E0F0[];
extern int float_to_int(float);

void func_001E13E0(int arg0) {
    char *base;
    char *a2;
    char *p;
    char *s2;
    char *s3;
    char *s4;
    char *s5;
    char *s7;
    int i;
    int j;
    int s1;
    float f20;

    s5 = (char *)D_0081E0F0;
    i = 0;
    do {
        base = D_00275670;
        a2 = base + arg0 * 4;
        *(char *)(*(char **)(a2 + 0x10) + 3) = 0x10;
        *(int *)(*(char **)(a2 + 0x10) + 4) = 0;
        *(short *)(*(char **)(a2 + 0x10) + 0) = 0x68;
        p = *(char **)(a2 + 0x10);
        *(char **)(a2 + 0x10) = p + 0x690;

        if (i == 0xF) {
            f20 = 1.0f;
            s1 = 0x1000;
        } else {
            f20 = 0.0f;
            s1 = 0;
        }

        *(long long *)(p + 0x10) = 0;
        *(long long *)(p + 0x18) = 0;
        *(int *)(p + 0x1C) = 0x67 | 0x50000000;
        *(long long *)(p + 0x20) = (long long)0x8011 | ((long long)0x600A4000 << 32);
        *(long long *)(p + 0x28) = 0x512512;

        s7 = (char *)D_0081E0F0 + (((i + 1) & 0xF) * 0x180);
        s2 = p + 0x30;
        s4 = s7;
        j = 0;
        s3 = s5;
        do {
            *(float *)(s2 + 0x0) = *(float *)(s3 + 0x10);
            *(float *)(s2 + 0x4) = *(float *)(s3 + 0x14);
            *(int *)(s2 + 0x8) = 0x3F800000;
            *(int *)(s2 + 0x10) = 0x40;
            *(int *)(s2 + 0x14) = 0x40;
            *(int *)(s2 + 0x18) = 0x40;
            *(int *)(s2 + 0x1C) = 0x80;
            *(int *)(s2 + 0x20) = float_to_int(30720.0f + (16.0f * *(float *)(s3 + 0x0)));
            *(int *)(s2 + 0x24) = float_to_int(30720.0f + (16.0f * *(float *)(s3 + 0x4)));
            *(int *)(s2 + 0x28) = 0;
            *(int *)(s2 + 0x2C) = 0;
            *(float *)(s2 + 0x30) = *(float *)(s4 + 0x10);
            *(float *)(s2 + 0x34) = f20 + *(float *)(s4 + 0x14);
            *(int *)(s2 + 0x38) = 0x3F800000;
            *(int *)(s2 + 0x40) = 0x40;
            *(int *)(s2 + 0x44) = 0x40;
            *(int *)(s2 + 0x48) = 0x40;
            *(int *)(s2 + 0x4C) = 0x80;
            *(int *)(s2 + 0x50) = float_to_int(30720.0f + (16.0f * *(float *)(s4 + 0x0)));
            *(int *)(s2 + 0x54) = float_to_int((float)s1 + (30720.0f + (16.0f * *(float *)(s4 + 0x4))));
            *(int *)(s2 + 0x58) = 0;
            *(int *)(s2 + 0x5C) = 0;
            j++;
            s2 += 0x60;
            s3 += 0x18;
            s4 += 0x18;
        } while (j < 16);

        *(float *)(s2 + 0x0) = 1.0f + *(float *)(s5 + 0x10);
        *(float *)(s2 + 0x4) = *(float *)(s5 + 0x14);
        *(int *)(s2 + 0x8) = 0x3F800000;
        *(int *)(s2 + 0x10) = 0x40;
        *(int *)(s2 + 0x14) = 0x40;
        *(int *)(s2 + 0x18) = 0x40;
        *(int *)(s2 + 0x1C) = 0x80;
        *(int *)(s2 + 0x20) = 0x8800;
        *(int *)(s2 + 0x24) = float_to_int(30720.0f + (16.0f * *(float *)(s5 + 0x4)));
        *(int *)(s2 + 0x28) = 0;
        *(int *)(s2 + 0x2C) = 0;
        *(float *)(s2 + 0x30) = 1.0f + *(float *)(s7 + 0x10);
        *(float *)(s2 + 0x34) = f20 + *(float *)(s7 + 0x14);
        *(int *)(s2 + 0x38) = 0x3F800000;
        *(int *)(s2 + 0x40) = 0x40;
        *(int *)(s2 + 0x44) = 0x40;
        *(int *)(s2 + 0x48) = 0x40;
        *(int *)(s2 + 0x4C) = 0x80;
        *(int *)(s2 + 0x50) = 0x8800;
        i++;
        *(int *)(s2 + 0x54) = float_to_int((float)s1 + (30720.0f + (16.0f * *(float *)(s7 + 0x4))));
        *(int *)(s2 + 0x58) = 0;
        s5 += 0x180;
        *(int *)(s2 + 0x5C) = 0;
    } while (i < 16);
}

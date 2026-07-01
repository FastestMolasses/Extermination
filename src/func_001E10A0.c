// NEARMISS func_001E10A0  (vram 0x001E10A0, 0x340 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 60.43% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Cross-compiler FP list-scheduling divergence: mwcc233/991202's -O4,p scheduler hoists the loop-invariant (float)i/16.0f divide (mtc1/cvt.s.w/div.s) to the very start of each outer-loop iteration regardless of source statement placement, while the target pads with 2 explicit nops and schedules it ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS: water/heat diffusion step over the 16x16 D_0081E0F0 grid (0x18-byte
// records, same layout func_001E1010 initializes: [0]=fi,[4]=fo,[8]=height,
// [0xC]=velocity). Outer pass diffuses velocity from the 4 wrapped neighbors and
// derives per-cell flow fields at [0x10]/[0x14]. Then two PRNG-picked "splash" cells
// (a,b) in the sibling D_0081E0FC grid get an impulse added to their center + 8
// neighbors (1.2f center, 0.9f orthogonal, 0.54f diagonal). Final pass integrates
// height += velocity across D_0081E0F0. Sole residual: mwcc's -O4,p scheduler
// hoists the per-row `(float)i/16.0f` div (mtc1/cvt.s.w/div.s) to the very start of
// the outer-loop body regardless of source placement/barriers, while the target
// pads with 2 nops and issues it after the pjm1/pjp1 pointer setup — a genuine
// cross-compiler list-scheduling divergence (not idiom-13; source reordering,
// asm volatile barriers, and -O4/-O4,s alternates all fail to reproduce it).
extern unsigned char D_0081E0F0[];
extern unsigned char D_0081E0FC[];
extern int func_00122BB8(void);

void func_001E10A0(void) {
    int i;
    int j;
    char *row;
    char *e;
    int jp1;
    char *pjm1;
    char *pjp1;
    float finv;
    float sum;
    int r;
    int a;
    int b;
    char *pa;
    char *pb;

    i = 0;
    row = (char *)D_0081E0F0;
    do {
        pjm1 = (char *)D_0081E0F0 + (((i - 1) & 0xF) * 0x180);
        pjp1 = (char *)D_0081E0F0 + (((i + 1) & 0xF) * 0x180);
        j = 0;
        e = row;
        finv = ((float)i) / 16.0f;
        do {
            sum = 0.0f + *(float *)(pjm1 + 8) + *(float *)(pjp1 + 8);
            pjm1 += 0x18;
            jp1 = (j + 1) & 0xF;
            pb = row + (jp1 * 0x18);
            *(float *)(e + 0xC) = *(float *)(e + 0xC) * 0.961f;
            *(float *)(e + 0xC) = *(float *)(e + 0xC) + (0.0022f *
                ((sum + *(float *)(row + (((j - 1) & 0xF) * 0x18) + 8) + *(float *)(pb + 8))
                 - (0.25f + (4.0f * *(float *)(e + 8)))));
            *(float *)(e + 0x10) = (((float)j) / 16.0f) + (0.03f * (*(float *)(e + 8) - *(float *)(pb + 8)));
            *(float *)(e + 0x14) = finv + (0.03f * (*(float *)(e + 8) - *(float *)(pjp1 + 8)));
            pjp1 += 0x18;
            e += 0x18;
            j++;
        } while (j < 16);
        i++;
        row += 0x180;
    } while (i < 16);

    a = (func_00122BB8() >> 6) & 0xF;
    b = (func_00122BB8() >> 6) & 0xF;
    pa = (char *)D_0081E0FC + (b * 0x180);
    *(float *)(pa + (a * 0x18)) += 1.2f;
    *(float *)(pa + (((a - 1) & 0xF) * 0x18)) += 0.90000004f;
    *(float *)(pa + (((a + 1) & 0xF) * 0x18)) += 0.90000004f;
    pb = (char *)D_0081E0FC + (((b - 1) & 0xF) * 0x180);
    *(float *)(pb + (a * 0x18)) += 0.90000004f;
    *(float *)(pb + (((a - 1) & 0xF) * 0x18)) += 0.54f;
    *(float *)(pb + (((a + 1) & 0xF) * 0x18)) += 0.54f;
    pa = (char *)D_0081E0FC + (((b + 1) & 0xF) * 0x180);
    *(float *)(pa + (a * 0x18)) += 0.90000004f;
    *(float *)(pa + (((a - 1) & 0xF) * 0x18)) += 0.54f;
    *(float *)(pa + (((a + 1) & 0xF) * 0x18)) += 0.54f;

    r = 0;
    e = (char *)D_0081E0F0;
    do {
        j = 0;
        pa = e;
        do {
            j++;
            *(float *)(pa + 8) = *(float *)(pa + 8) + *(float *)(pa + 0xC);
            pa += 0x18;
        } while (j < 16);
        r++;
        e += 0x180;
    } while (r < 16);
}

// NEARMISS func_00208750  (vram 0x00208750, 0x35C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.81% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a compiler register-colouring choice no source change tried here fixes:
// Remaining differences: two caller-saved temporaries swap (the per-strip display-list base pointer
// and the 1.0f constant for the step division); size, schedule and every other register are identical.
// Declaration-order, statement-order and type sweeps leave it unchanged.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// draw2d: builds 9 GIF/DMA-tagged vertex strips (a quadratic-Bezier "ribbon")
// into the display-list bump allocator at D_00275670->cur (+0x14). The 9-entry
// table of (dx, dy) int offset pairs at D_00265160 (0x48 bytes) is copied into a
// local by one struct assignment. For each strip j: tag byte +3 = 0x10, word
// +4 = 0, half +0 = 2n+2, the cursor advances by (2n+3)*16; then a header
// (zeroed quadword, word +0x1C = (2n+1)|0x50000000, dword +0x20 =
// n | 0x8000 | 0x20254000<<32, dword +0x28 = 0x41) and n vertices of 8 words.
// t runs 0, 1/(n-1), ... (reset for every strip); with u = 1-t the weights are
// u*u, 2*(u*t), t*t over 3 control rows of 4 ints (arg2, stride 16 bytes) for
// words 0..3 (each halved when the strip's offset pair is non-zero), and over
// 3 float rows (arg1) plus the strip's dx/dy for words 4..5; word 6 = 0xFFFFFF,
// word 7 = 0. The caller func_00209DF0 passes n = 0x10.
extern int float_to_int(float);
typedef struct {
    int v[9][2];
} OfsTable;
extern OfsTable D_00265160;
typedef struct {
    char pad[0x14];
    char *cur;
} DispList;
extern DispList *D_00275670;

void func_00208750(int n, float *arg1, int *arg2) {
    OfsTable ofs;
    unsigned short cnt;
    int size;
    int flags;
    long long hdr2;
    int j, i;
    int s1, s2;
    int *dst;
    char *cursor;
    DispList *dl;
    float t;
    float invstep;

    j = 0;
    ofs = D_00265160;
    cnt = n * 2 + 2;
    size = (n * 2 + 3) * 0x10;
    flags = (n * 2 + 1) | 0x50000000;
    hdr2 = (long long)n | 0x8000 | ((long long)0x20254000 << 32);

    for (; j < 9; j++) {
        dl = D_00275670;
        s1 = ofs.v[j][0];
        s2 = ofs.v[j][1];
        invstep = 1.0f / (float)(n - 1);
        t = 0.0f;
        *(unsigned char *)(dl->cur + 3) = 0x10;
        *(int *)(dl->cur + 4) = 0;
        *(unsigned short *)(dl->cur) = cnt;
        cursor = dl->cur;
        dst = (int *)(cursor + 0x10);
        dl->cur = cursor + size;
        *(unsigned __int128 *)(cursor + 0x10) = 0;
        *(int *)(cursor + 0x1C) = flags;
        *(long long *)(cursor + 0x20) = hdr2;
        *(long long *)(cursor + 0x28) = 0x41;
        dst += 8;
        for (i = 0; i < n; i++) {
            float d, w22, w23, w24;
            d = 1.0f - t;
            w22 = d * d;
            w23 = 2.0f * (d * t);
            w24 = t * t;
            dst[0] = float_to_int((float)arg2[0] * w22 + (float)arg2[4] * w23 + (float)arg2[8] * w24);
            dst[1] = float_to_int((float)arg2[1] * w22 + (float)arg2[5] * w23 + (float)arg2[9] * w24);
            dst[2] = float_to_int((float)arg2[2] * w22 + (float)arg2[6] * w23 + (float)arg2[10] * w24);
            dst[3] = float_to_int((float)arg2[3] * w22 + (float)arg2[7] * w23 + (float)arg2[11] * w24);
            if (s1 != 0 || s2 != 0) {
                dst[0] = dst[0] >> 1;
                dst[1] = dst[1] >> 1;
                dst[2] = dst[2] >> 1;
                dst[3] = dst[3] >> 1;
            }
            dst[4] = float_to_int((float)s1 + (arg1[0] * w22 + arg1[4] * w23 + arg1[8] * w24));
            dst[5] = float_to_int((float)s2 + (arg1[1] * w22 + arg1[5] * w23 + arg1[9] * w24));
            dst[6] = 0xFFFFFF;
            dst[7] = 0;
            t += invstep;
            dst += 8;
        }
    }
}

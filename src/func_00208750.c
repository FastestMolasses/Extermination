// NEARMISS func_00208750  (vram 0x00208750, 0x35C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 72.00% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Stack-frame-layout / base-pointer-sharing wall: target hoists &D_00265160 into ONE register and reuses it (lq/ld with offsets) for a one-time 0x48-byte struct copy; mwcc233 recomputes a fresh %hi/%lo(D_00265160+N) per field regardless of whether the source is a raw pointer, struct*, or array cast...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// draw2d: builds 9 GIF/DMA-tagged vertex strips (a "ribbon"/ninepatch-style GS
// primitive list) into the display-list bump allocator at D_00275670+0x14. A
// 0x48-byte source block (4 quadwords + 1 dword) is copied ONCE from the fixed
// table D_00265160 into a local scratch buffer; each of the 9 outer iterations
// re-reads a SLIDING 8-byte window into that same one-time-copied buffer (win+=8
// per iteration) to pull two per-strip int flags (s1/s2, used both as a
// "halve the XYZ" gate and as a per-vertex integer offset). For each of the 9
// strips: writes a GIF tag (nloop=cnt, eop/flags byte=0x10, word+4=0, half+0=cnt)
// then a 0x30-byte vertex-format header (zeroed quadword, flags word, two packed
// 64-bit PRIM/reg-descriptor words) followed by n vertices. Each vertex k (0<=k<n)
// evaluates a quadratic Bezier-style blend across arg2's 3 int rows (weights
// w22=(k-t)^2, w23=2*(k-t)*t, w24=t^2, t stepping by 1/(n-1) every vertex across
// ALL 9*n iterations, never reset) to produce XYZ (dst[0..3], optionally halved
// when s1|s2 != 0), then a second blend against arg1's rows offset by the per-
// strip int flags (dst[4..5] = s1/s2 + blend), followed by a constant color/pad
// word (0xFFFFFF) and a zero pad (dst[6..7]).
extern int float_to_int(float);
typedef unsigned __int128 uint128;
extern uint128 D_00265160[5];
extern char *D_00275670;

void func_00208750(int n, char *arg1, char *arg2) {
    char buf[0x50];
    unsigned short cnt;
    int size;
    int flags;
    long long hdr2;
    int j, i;
    int s1, s2;
    int *dst;
    char *cursor;
    char *out;
    float invstep;
    float t;
    char *win;
    char *src;

    src = (char *)D_00265160;
    cnt = n * 2 + 2;
    size = (n * 2 + 3) * 0x10;
    *(uint128 *)(buf + 0) = *(uint128 *)(src + 0);
    *(uint128 *)(buf + 0x10) = *(uint128 *)(src + 0x10);
    *(uint128 *)(buf + 0x20) = *(uint128 *)(src + 0x20);
    *(uint128 *)(buf + 0x30) = *(uint128 *)(src + 0x30);
    flags = (n * 2 + 1) | 0x50000000;
    *(long long *)(buf + 0x40) = *(long long *)(src + 0x40);
    hdr2 = ((long long)(unsigned int)0x20254000 << 0x20) | 0x8000 | (unsigned int)n;

    t = 0.0f;
    win = buf;
    for (j = 0; j < 9; j++) {
        cursor = *(char **)(D_00275670 + 0x14);
        s1 = *(int *)win;
        s2 = *(int *)(win + 4);
        *(unsigned char *)(cursor + 3) = 0x10;
        invstep = 1.0f / (float)(n - 1);
        cursor = *(char **)(D_00275670 + 0x14);
        *(int *)(cursor + 4) = 0;
        cursor = *(char **)(D_00275670 + 0x14);
        *(unsigned short *)cursor = cnt;
        cursor = *(char **)(D_00275670 + 0x14);
        out = cursor + 0x10;
        *(char **)(D_00275670 + 0x14) = cursor + size;
        *(uint128 *)out = 0;
        dst = (int *)(out + 0x20);
        *(int *)(out + 0x1C) = flags;
        *(long long *)(out + 0x20) = hdr2;
        *(long long *)(out + 0x28) = 0x41;

        if (n > 0) {
            for (i = 0; i < n; i++) {
                float d, w22, w23, w24;
                d = (float)i - t;
                w22 = d * d;
                w23 = 2.0f * (d * t);
                w24 = t * t;
                dst[0] = float_to_int(
                    (float)*(int *)(arg2 + 0) * w22
                    + (float)*(int *)(arg2 + 0x10) * w23
                    + (float)*(int *)(arg2 + 0x20) * w24);
                dst[1] = float_to_int(
                    (float)*(int *)(arg2 + 4) * w22
                    + (float)*(int *)(arg2 + 0x14) * w23
                    + (float)*(int *)(arg2 + 0x24) * w24);
                dst[2] = float_to_int(
                    (float)*(int *)(arg2 + 8) * w22
                    + (float)*(int *)(arg2 + 0x18) * w23
                    + (float)*(int *)(arg2 + 0x28) * w24);
                dst[3] = float_to_int(
                    (float)*(int *)(arg2 + 0xC) * w22
                    + (float)*(int *)(arg2 + 0x1C) * w23
                    + (float)*(int *)(arg2 + 0x2C) * w24);
                if (s1 != 0 || s2 != 0) {
                    dst[0] = dst[0] >> 1;
                    dst[1] = dst[1] >> 1;
                    dst[2] = dst[2] >> 1;
                    dst[3] = dst[3] >> 1;
                }
                dst[4] = float_to_int(
                    (float)s1
                    + ((float)*(int *)(arg1 + 0) * w22
                       + (float)*(int *)(arg1 + 0x10) * w23
                       + (float)*(int *)(arg1 + 0x20) * w24));
                dst[5] = float_to_int(
                    (float)s2
                    + ((float)*(int *)(arg1 + 4) * w22
                       + (float)*(int *)(arg1 + 0x14) * w23
                       + (float)*(int *)(arg1 + 0x24) * w24));
                dst[6] = 0xFFFFFF;
                dst[7] = 0;
                t += invstep;
                dst += 8;
            }
        }
        win += 8;
    }
}

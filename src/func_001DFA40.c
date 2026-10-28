// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 98.55% NEARMISS). Fixes read from the original instructions:
// - the row y coordinate is a plain signed (x/15 + 0x790)*16; the texture table is reached by absolute address (unsized array); the packet header uses the DMA packet family spelling (cursor re-read per store); lo[] is filled before hi[].
// - the vertex cursor starts at blk+0x10 and moves past the 0x20-byte GIF header between the lo[] and hi[] fills; the grid rows are walked through two row pointers set up after j = 0; each int coordinate goes through a float temporary.
// - the X step 546.13336f (8192/15) is a named local assigned inside the inner loop, which fixes the operand order of the two adds before float_to_int.
// - the warp sum (su+sv) is its own named temporary before the multiply by fparg0, which fixes that multiply's operand order.
//
// Builds a 16x16 grid of (u', v', 1.0) texture coordinates warped by a radial term
// (k = 0.5 / (1 + (f(2u-1)^2 + f(2v-1)^2) * fparg0)), then emits 15 GS strips of 16
// vertex pairs (0x430-byte packets) through the per-context cursor D_00275670[arg0].slot[4],
// with screen X stepping by 8192/15 and Y rows at (row*224/15 + 0x790)*16. Returns the
// cursor value read on entry.

typedef unsigned __int128 u128;
extern void func_001D6B60(int a0, int a1, int a2, int a3, void *a4);
extern void func_001D6BA0(int a0, int a1, int a2, int a3, int a4, int a5);
extern void func_001D1FF0(int a0, int a1);
extern void func_001D2040(int a0, int a1);
extern void func_001D7080(int a0, int a1, float fa0);
extern float func_0011DF78(float a0);
extern void func_001D1F20(int a0);
extern void func_00102948(void *a0, void *a1);
extern int float_to_int(float);

extern char **D_00275670;
extern int D_0027568C;
extern int D_0026E880[];

int func_001DFA40(int arg0, int arg1, long long arg2, float fparg0) {
    float tab[16][16][4];
    int lo[4];
    int hi[4];
    int j;
    int i;
    int ret;
    float u;
    float v;
    float su;
    float sv;
    float k;
    float t;
    float x;
    float d;
    float step;
    char **slot;
    char *blk;
    char *vb;
    float *p;
    float *q;

    ret = (int)D_00275670[arg0 + 4];
    func_001D6B60(arg0, D_0027568C, 8, 8, D_0026E880);
    func_001D6BA0(arg0, D_0027568C, 8, 8, 0, 0);
    func_001D1FF0(arg0, 3);
    func_001D2040(arg0, 0);
    func_001D7080(arg0, (int)arg2, 1.0f);

    for (i = 0; i < 16; i++) {
        v = (float)i / 15.0f;
        t = func_0011DF78(2.0f * v - 1.0f);
        sv = t * t;
        for (j = 0; j < 16; j++) {
            u = (float)j / 15.0f;
            su = func_0011DF78(2.0f * u - 1.0f);
            su = su * su;
            d = su + sv;
            k = d * fparg0;
            k = 0.5f / (1.0f + k);
            tab[i][j][0] = 0.5f + (u - 0.5f) * k;
            tab[i][j][1] = 0.5f + (v - 0.5f) * k;
            tab[i][j][2] = 1.0f;
        }
    }

    for (i = 0; i < 15; i++) {
        slot = (char **)(&D_00275670[arg0]);
        *((char *)(slot[4] + 3)) = 0x10;
        *((int *)(slot[4] + 4)) = 0;
        *((short *)(slot[4] + 0)) = 0x42;
        blk = slot[4];
        slot[4] = blk + 0x430;
        *((u128 *)(blk + 0x10)) = 0;
        *((int *)(blk + 0x1C)) = 0x50000041;
        *((long long *)(blk + 0x20)) =
            (((long long)((int)(arg1 | 0x14))) << 0x2F) | (0x8010 | (((long long)0x40004000) << 0x20));
        *((long long *)(blk + 0x28)) = 0x4242;
        vb = blk + 0x10;
        lo[0] = 0x7000;
        lo[1] = ((i * 0xE0) / 15 + 0x790) * 0x10;
        lo[2] = 0xFFFFFF;
        lo[3] = 0;
        vb += 0x20;
        hi[0] = 0x7000;
        hi[1] = (((i + 1) * 0xE0) / 15 + 0x790) * 0x10;
        hi[2] = 0xFFFFFF;
        hi[3] = 0;
        j = 0;
        p = tab[i][0];
        q = tab[i + 1][0];
        for (; j < 16; j++) {
            func_00102948(vb, p);
            func_00102948(vb + 0x10, lo);
            func_00102948(vb + 0x20, q);
            func_00102948(vb + 0x30, hi);
            step = 546.13336f;
            x = (float)lo[0];
            lo[0] = float_to_int(x + step);
            x = (float)hi[0];
            hi[0] = float_to_int(x + step);
            p += 4;
            q += 4;
            vb += 0x40;
        }
    }

    func_001D1F20(arg0);
    func_001D1FF0(arg0, 1);
    return ret;
}

// NEARMISS func_001DFA40  (vram 0x001DFA40, 0x3CC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.55% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful.
// Matching wave 2026-10-27 (was 71.44%): the row y coordinate is a plain signed (x/15 + 0x790)*16
// (the old C added a second sign term); D_0026E880 is reached by absolute address (unsized array);
// the packet header uses the DMA packet family spelling (cursor re-read per store); lo[] is filled
// before hi[]; i is declared before j.
// Remaining differences: the (su+sv)*fparg0 multiply's operand order, the vertex cursor
// blk+0x10+0x20 folded to one add (original adds 0x10 then 0x20 later), FP register choice
// of the 546.13336f add before float_to_int, and the inner loop's pointer-increment order.
//
// Builds a 16x16 grid of (u', v', 1.0) texture coordinates warped by a radial term
// (k = 0.5 / (1 + (f(2u-1)^2 + f(2v-1)^2) * fparg0)), then emits 15 GS strips of 16
// vertex pairs (0x430-byte packets) through the per-context cursor D_00275670[arg0].slot[4],
// with screen X stepping by 8192/15 and Y rows at (row*224/15 + 0x790)*16. Returns the
// cursor value read on entry.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

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
    char **slot;
    char *blk;
    char *vb;

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
            k = (su + sv) * fparg0;
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
        lo[0] = 0x7000;
        lo[1] = ((i * 0xE0) / 15 + 0x790) * 0x10;
        lo[2] = 0xFFFFFF;
        lo[3] = 0;
        hi[0] = 0x7000;
        hi[1] = (((i + 1) * 0xE0) / 15 + 0x790) * 0x10;
        hi[2] = 0xFFFFFF;
        hi[3] = 0;
        vb = blk + 0x10 + 0x20;
        for (j = 0; j < 16; j++) {
            func_00102948(vb, tab[i][j]);
            func_00102948(vb + 0x10, lo);
            func_00102948(vb + 0x20, tab[i + 1][j]);
            func_00102948(vb + 0x30, hi);
            lo[0] = float_to_int((float)lo[0] + 546.13336f);
            hi[0] = float_to_int((float)hi[0] + 546.13336f);
            vb += 0x40;
        }
    }

    func_001D1F20(arg0);
    func_001D1FF0(arg0, 1);
    return ret;
}

// NEARMISS func_001DF180  (vram 0x001DF180, 0x41C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 81.46% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful.
// Matching wave 2026-10-27 (was 66.41%), spelled like its twin func_001DFA40: the packet header
// uses the DMA packet family spelling (slot = &D_00275670[arg0], cursor slot[4] re-read per store);
// j is declared before i; the outer squared term goes through a temporary; the radial term is
// k = sv + su; k = k * fparg0; the two strip rows (lo/hi) are filled interleaved; the vertex
// cursor advances in the for-increment. The 0x10F0 frame now matches (the old "frame 16 bytes
// larger" verdict was a spelling artifact).
// Remaining differences: instruction scheduling and FP register choice around the two random
// jitter products, scheduling of the packet-header stores against the row-Y divisions, the vertex
// cursor blk+0x10+0x20 folded to one add (original adds 0x10 then 0x20 later), and the FP
// registers of the 546.13336f add before float_to_int.
//
// Builds a 16x16 grid of (u', v', 1.0) texture coordinates: u = j/15, v = i/15, with a jitter
// k * (2*rand*2^-31 - 1) per axis where k = (f(2u-1)^2 + f(2v-1)^2) * fparg0 (func_0011DF78 is
// the sinf-family helper, func_00122BB8 the 31-bit random source), plus the offsets 2**-9 (x) and
// about 1/224 (y). Then emits 15 GS strips of 16 vertex pairs (0x430-byte packets) through the
// per-context cursor D_00275670[arg0].slot[4], with screen X stepping by 8192/15 and Y rows at
// (row*224/15 + 0x790)*16. Returns the cursor value read on entry.
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
extern int func_00122BB8(void);
extern void func_001D1F20(int a0);
extern void func_00102948(void *a0, void *a1);
extern int float_to_int(float);

extern char **D_00275670;
extern int D_0027568C;
extern int D_0026E860[];

int func_001DF180(int arg0, float fparg0) {
    float tab[16][16][4];
    int lo[4];
    int hi[4];
    int j;
    int i;
    int ret;
    float v;
    float u;
    float k;
    float su;
    float sv;
    float t;
    float r1;
    float r2;
    char **slot;
    char *blk;
    char *vb;

    ret = (int)D_00275670[arg0 + 4];
    func_001D6B60(arg0, D_0027568C, 8, 8, D_0026E860);
    func_001D6BA0(arg0, D_0027568C, 8, 8, 0, 0);
    func_001D1FF0(arg0, 3);
    func_001D2040(arg0, 0);
    func_001D7080(arg0, 0x80808080, 1.0f);

    for (i = 0; i < 16; i++) {
        v = (float)i / 15.0f;
        t = func_0011DF78(2.0f * v - 1.0f);
        su = t * t;
        for (j = 0; j < 16; j++) {
            u = (float)j / 15.0f;
            sv = func_0011DF78(2.0f * u - 1.0f);
            sv = sv * sv;
            k = sv + su;
            k = k * fparg0;
            r1 = 4.656613e-10f * (float)func_00122BB8();
            r2 = 4.656613e-10f * (float)func_00122BB8();
            r1 = 2.0f * r1;
            r2 = 2.0f * r2;
            tab[i][j][0] = 0.001953125f + (u + k * (-1.0f + r1));
            tab[i][j][1] = 0.004464286f + (v + k * (-1.0f + r2));
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
        *((long long *)(blk + 0x20)) = 0x8010 | (((long long)0x400A4000) << 0x20);
        *((long long *)(blk + 0x28)) = 0x4242;
        lo[0] = 0x7000;
        hi[0] = 0x7000;
        lo[1] = ((i * 0xE0) / 15 + 0x790) * 0x10;
        hi[1] = (((i + 1) * 0xE0) / 15 + 0x790) * 0x10;
        lo[2] = 0xFFFFFF;
        hi[2] = 0xFFFFFF;
        lo[3] = 0;
        hi[3] = 0;
        vb = blk + 0x10 + 0x20;
        for (j = 0; j < 16; j++, vb += 0x40) {
            func_00102948(vb, tab[i][j]);
            func_00102948(vb + 0x10, lo);
            func_00102948(vb + 0x20, tab[i + 1][j]);
            func_00102948(vb + 0x30, hi);
            lo[0] = float_to_int((float)lo[0] + 546.13336f);
            hi[0] = float_to_int((float)hi[0] + 546.13336f);
        }
    }

    func_001D1F20(arg0);
    func_001D1FF0(arg0, 1);
    return ret;
}

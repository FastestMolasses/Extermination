// NEARMISS func_001DF180  (vram 0x001DF180, 0x41C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 57.19% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation / stack-frame-size wall in a very large function (10 saved GPR incl. $fp + 6 saved FPR, 0x10F0-byte frame in target). This compile's frame is 16 bytes larger (0x1100) because the compiler spills arg0 to the stack instead of matching the target's choice to spill the pre-loop 'r...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// weapon_equip: builds a 16x16 randomized-jitter UV/position grid (func_0011DF78
// = sinf-family), then emits 15 rows of GS packets (16 quads/row, two texture-
// coord scratch vec4s stepped via float_to_int(cur+546.13336f) per column) into
// the actor's DMA ring at D_00275670[arg0]->+0x10. Logic/structure fully
// recovered from the splat .s; residual is register-allocation/frame-size in
// this large (0x10F0-byte-frame, 10 saved GPR + 6 saved FPR) function.
//
extern int float_to_int(float f);
extern void func_00102948(void *out, void *a);
extern float func_0011DF78(float rad);
extern int func_00122BB8(void);
extern void func_001D1F20(int actor);
extern void func_001D1FF0(int actor, int n);
extern void func_001D2040(int actor, int n);
extern void func_001D6B60(int, int, int);
extern void func_001D6BA0(int, int, int, int);
extern void func_001D7080(int, int, int);

extern char *D_00275670;
extern int D_0027568C;

int func_001DF180(int arg0, float fparg0) {
    int idx = arg0 * 4;
    int result = *(int *)(D_00275670 + idx + 0x10);
    float grid[16 * 16][4];
    int i, j;

    func_001D6B60(D_0027568C, 8, 8);
    func_001D6BA0(arg0, D_0027568C, 8, 8);
    func_001D1FF0(arg0, 3);
    func_001D2040(arg0, 0);
    func_001D7080(arg0, 0x80808080, 0x3F800000);

    for (i = 0; i < 0x10; i++) {
        float u = (float) i / 15.0f;
        float su = func_0011DF78((2.0f * u) - 1.0f);
        float su2 = su * su;
        for (j = 0; j < 0x10; j++) {
            float v = (float) j / 15.0f;
            float sv = func_0011DF78((2.0f * v) - 1.0f);
            float h = ((sv * sv) + su2) * fparg0;
            float rnd1 = 4.656613e-10f * (float) func_00122BB8();
            float rnd2 = 4.656613e-10f * (float) func_00122BB8();
            grid[i * 16 + j][0] = v + (h * (-1.0f + (2.0f * rnd1)));
            grid[i * 16 + j][1] = u + (h * (-1.0f + (2.0f * rnd2)));
            *(int *)&grid[i * 16 + j][2] = 0x3F800000;
        }
    }

    {
        unsigned int rowbytes = 0;
        for (i = 0; i < 0xF; i++) {
            char *owner = D_00275670 + idx;
            char *rec = *(char **)(owner + 0x10);
            int n1 = i + 1;
            int stcur[4];
            int stnext[4];
            char *out;

            *(unsigned char *)(rec + 3) = 0x10;
            *(int *)(rec + 4) = 0;
            *(short *)(rec + 0) = 0x42;
            *(char **)(owner + 0x10) = rec + 0x430;
            *(long long *)(rec + 0x10) = 0;
            *(long long *)(rec + 0x18) = 0;
            *(int *)(rec + 0x1C) = 0x50000041;
            *(unsigned long long *)(rec + 0x20) = 0x8010ULL | ((unsigned long long) 0x400A4000 << 32);
            *(long long *)(rec + 0x28) = 0x4242;

            stcur[0] = 0x7000;
            stcur[1] = ((rowbytes / 15) + ((int) rowbytes >> 31) + 0x790) << 4;
            stcur[2] = 0xFFFFFF;
            stcur[3] = 0;

            stnext[0] = 0x7000;
            stnext[1] = ((((unsigned int)(n1 * 0xE0)) / 15) + (((unsigned int)(n1 * 0xE0)) >> 31) + 0x790) << 4;
            stnext[2] = 0xFFFFFF;
            stnext[3] = 0;

            out = rec + 0x30;
            for (j = 0; j < 0x10; j++) {
                func_00102948(out, grid[i * 16 + j]);
                func_00102948(out + 0x10, stcur);
                func_00102948(out + 0x20, grid[n1 * 16 + j]);
                func_00102948(out + 0x30, stnext);
                stcur[0] = float_to_int((float) stcur[0] + 546.13336f);
                stnext[0] = float_to_int((float) stnext[0] + 546.13336f);
                out += 0x40;
            }
            rowbytes += 0xE0;
        }
    }

    func_001D1F20(arg0);
    func_001D1FF0(arg0, 1);
    return result;
}

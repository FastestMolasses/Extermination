// NEARMISS func_001DF180  (vram 0x001DF180, 0x41C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 66.41% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation / stack-frame-size wall in a very large function (10 saved GPR incl. $fp + 6 saved FPR, 0x10F0-byte frame in target). This compile's frame is 16 bytes larger (0x1100) because the compiler spills arg0 to the stack instead of matching the target's choice to spill the pre-loop 'r...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// CORRECTED (lane NMFIX4, 2026-09-28): func_001D6B60 takes five arguments (arg0, D_0027568C, 8, 8,
// &D_0026E860); func_001D6BA0 six (t0 = t1 = 0); func_001D7080's third argument is the float 1.0 in f12
// (the old C passed the int 0x3F800000 and left f12 unset); grid x / y carry the final offsets 2**-9 and
// 0x3B924925 (about 1/224); the row header re-reads the cursor word before each of its first four
// accesses; the ST row value is the signed (i * 0xE0) / 15. Written from the loop index: a signed
// accumulator (rowbytes += 0xE0; rowbytes / 15) makes mwcc 2.3.3 step the quotient by +14 (wrong from row 2).
// Checked by an original-instruction harness over the captured AREA00 RAM; objdiff 57.19% -> 66.41%.
// Grid w words are never written (the original copies whatever the stack held).
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
extern int func_001D6B60(int a0, int a1, int a2, int a3, void *a4);
extern void func_001D6BA0(int a0, int a1, int a2, int a3, int t0, int t1);
extern void func_001D7080(int a0, int a1, float f);

extern char *D_00275670;
extern int D_0027568C;
extern char D_0026E860[];

int func_001DF180(int arg0, float fparg0) {
    int idx = arg0 * 4;
    int result = *(int *)(D_00275670 + idx + 0x10);
    float grid[16 * 16][4];
    int i, j;

    func_001D6B60(arg0, D_0027568C, 8, 8, D_0026E860);
    func_001D6BA0(arg0, D_0027568C, 8, 8, 0, 0);
    func_001D1FF0(arg0, 3);
    func_001D2040(arg0, 0);
    func_001D7080(arg0, 0x80808080, 1.0f);

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
            grid[i * 16 + j][0] = 0.001953125f + (v + (h * (-1.0f + (2.0f * rnd1))));
            grid[i * 16 + j][1] = 0.004464286f + (u + (h * (-1.0f + (2.0f * rnd2))));
            *(int *)&grid[i * 16 + j][2] = 0x3F800000;
        }
    }

    for (i = 0; i < 0xF; i++) {
        char *owner = D_00275670 + idx;
        char *rec;
        int n1 = i + 1;
        int stcur[4];
        int stnext[4];
        char *out;

        /* the original re-reads the cursor word before each of these */
        *(unsigned char *)(*(char **)(owner + 0x10) + 3) = 0x10;
        *(int *)(*(char **)(owner + 0x10) + 4) = 0;
        *(short *)(*(char **)(owner + 0x10) + 0) = 0x42;
        rec = *(char **)(owner + 0x10);
        *(char **)(owner + 0x10) = rec + 0x430;
        *(long long *)(rec + 0x10) = 0;
        *(long long *)(rec + 0x18) = 0;
        *(int *)(rec + 0x1C) = 0x50000041;
        *(unsigned long long *)(rec + 0x20) = 0x8010ULL | ((unsigned long long) 0x400A4000 << 32);
        *(long long *)(rec + 0x28) = 0x4242;

        stcur[0] = 0x7000;
        stcur[1] = (((i * 0xE0) / 15) + 0x790) << 4;
        stcur[2] = 0xFFFFFF;
        stcur[3] = 0;

        stnext[0] = 0x7000;
        stnext[1] = (((n1 * 0xE0) / 15) + 0x790) << 4;
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
    }

    func_001D1F20(arg0);
    func_001D1FF0(arg0, 1);
    return result;
}

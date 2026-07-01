// NEARMISS func_001DFA40  (vram 0x001DFA40, 0x3CC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 71.44% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Whole-function saved-register allocation ORDER permutation on both GP (s0-s7) and FP (f20-f23) saved registers vs the target's coloring, confirmed by the tail (last ~50 instructions, the second GS-emit loop's back half) being near-identical modulo register letters/branch offsets — proving the log...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

extern void func_001D6B60(int a0, int a1, int a2, int a3, void *a4);
extern void func_001D6BA0(int a0, int a1, int a2, int a3, int a4, int a5);
extern void func_001D1FF0(int a0, int a1);
extern void func_001D2040(int a0, int a1);
extern void func_001D7080(int a0, int a1, float fa0);
extern float func_0011DF78(float a0);
extern void func_001D1F20(int a0);
extern void func_00102948(void *a0, void *a1);
extern int float_to_int(float);

typedef int s128 __attribute__((mode(TI)));

extern char *D_00275670;
extern int D_0027568C;
extern int D_0026E880;

int func_001DFA40(int arg0, int arg1, long long arg2, float fparg0) {
    float tab[16][16][4];
    int scratch0[4];
    int scratch1[4];
    int i;
    int j;
    int ret;
    float u;
    float v;
    float su;
    float sv;
    float k;
    char *base;
    char *e;
    float *p;
    char *vb;
    int *uvp;
    long long mode;

    ret = *(int *)(D_00275670 + arg0 * 4 + 0x10);
    func_001D6B60(arg0, D_0027568C, 8, 8, &D_0026E880);
    func_001D6BA0(arg0, D_0027568C, 8, 8, 0, 0);
    func_001D1FF0(arg0, 3);
    func_001D2040(arg0, 0);
    func_001D7080(arg0, (int)arg2, 1.0f);

    for (i = 0; i < 16; i++) {
        v = (float) i / 15.0f;
        sv = func_0011DF78(2.0f * v - 1.0f);
        sv = sv * sv;
        for (j = 0; j < 16; j++) {
            u = (float) j / 15.0f;
            su = func_0011DF78(2.0f * u - 1.0f);
            su = su * su;
            k = 0.5f / (1.0f + (su + sv) * fparg0);
            tab[i][j][0] = 0.5f + (u - 0.5f) * k;
            tab[i][j][1] = 0.5f + (v - 0.5f) * k;
            *(int *)&tab[i][j][2] = 0x3F800000;
        }
    }

    mode = (long long)((arg1 | 0x14) << 0x20) >> 0x20;
    for (i = 0; i < 15; i++) {
        base = D_00275670 + arg0 * 4;
        e = *(char **)(base + 0x10);
        *(char *)(e + 3) = 0x10;
        *(int *)(e + 4) = 0;
        *(short *)(e + 0) = 0x42;
        *(char **)(base + 0x10) = e + 0x430;
        *(s128 *)(e + 0x10) = 0;
        *(int *)(e + 0x1C) = 0x50000041;
        *(long long *)(e + 0x20) = (mode << 0x2F) | ((long long)0x40004000 << 0x20) | 0x8010;
        *(long long *)(e + 0x28) = 0x4242;
        scratch0[0] = 0x7000;
        scratch1[0] = 0x7000;
        scratch0[1] = (((i * 0xE0) / 15) + ((unsigned int)(i * 0xE0) >> 31) + 0x790) * 0x10;
        scratch1[1] = ((((i + 1) * 0xE0) / 15) + ((unsigned int)((i + 1) * 0xE0) >> 31) + 0x790) * 0x10;
        scratch0[2] = 0xFFFFFF;
        scratch1[2] = 0xFFFFFF;
        scratch0[3] = 0;
        scratch1[3] = 0;
        vb = e + 0x10 + 0x20;
        uvp = (int *)tab[i + 1];
        p = (float *)tab[i];
        for (j = 0; j < 16; j++) {
            func_00102948(vb, p);
            func_00102948(vb + 0x10, scratch0);
            func_00102948(vb + 0x20, uvp);
            func_00102948(vb + 0x30, scratch1);
            scratch0[0] = float_to_int((float) scratch0[0] + 546.13336f);
            scratch1[0] = float_to_int((float) scratch1[0] + 546.13336f);
            p += 4;
            uvp += 4;
            vb += 0x40;
        }
    }

    func_001D1F20(arg0);
    func_001D1FF0(arg0, 1);
    return ret;
}

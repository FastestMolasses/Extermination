// NEARMISS func_001ECB00  (vram 0x001ECB00, 0x368 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 77.19% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-register-coloring / GPR-scheduling noise, not a missing/wrong operation: (1) the GS-alpha packed-constant OR-tree colors to a different scratch-register chain in every associativity/grouping tried; (2) each of the 3 clip-index-frac blocks (idx*37+11 GPR math interleaved with cvt.s.w/div.s/add....
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// GS/render-state pass for a pair of scene handles (arg0 = base struct, arg1 =
// submit index). If the global "scale" factor at D_00275C34+0x54 is below 1.0,
// builds a fade/alpha packet: computes 255/192-scaled byte components into the
// VU0 scratch words 0x70003600/04/08, packs them with alpha=0x80 into a 32-bit
// RGBA word, and pushes a scaled-radius quad via func_001CD520(mode=0,n=2,
// dst=arg0+0x30, packedConst=0x20045BA5154222DC, radius, radius, 6.0f, rgba).
// Then, three times, advances a per-entity PRNG-ish index at D_00275C34+4 by
// (idx*37+11), derives a 0..1 fractional value from its high 16 bits, and
// submits a billboard/particle packet via func_001CFB50(&D_0081F8F0, 0, arg0,
// scale, frac+0.0001, 1.0, 1e-6, 0.0) followed by func_001CFBE0(arg1, mode,
// table, &D_0081F8F0, 0) with mode/table = (0,D_00256EE0), (2,D_00256EE0),
// (1,D_00256F70). Finally smooths a damping factor at D_00275C34+8 toward
// 0.05 (exponential decay /10) and clamps it to a 0.05 floor.

extern int float_to_int(float);
extern void func_001CD520(int mode, int n, void *dst, long long packedConst, float f12, float f13, float f14, int packed);
extern void func_001CFB50(void *p, int a1, int a2, float f12, float f13, float f14, float f15, float f16);
extern void func_001CFBE0(int a0, int a1, void *a2, void *a3, int t0);
extern int D_00275C34;
extern unsigned char D_00256EE0[16];
extern unsigned char D_00256F70[16];
extern unsigned char D_0081F8F0[16];

void func_001ECB00(int arg0, int arg1) {
    float *scaleP;
    int idx;

    scaleP = (float *)(D_00275C34 + 0x54);
    if (*scaleP < 1.0f) {
        float diff = 1.0f - *scaleP;
        float f;
        int packed;
        *(int *)0x70003600 = float_to_int(255.0f * diff);
        *(int *)0x70003604 = float_to_int(192.0f * diff) << 8;
        *(int *)0x70003608 = float_to_int(192.0f * diff) << 16;
        packed = (0x80000000 | *(int *)0x70003608) | *(int *)0x70003604 | *(int *)0x70003600;
        f = 6.0f + (6.0f * *scaleP);
        func_001CD520(0, 2, (void *)(arg0 + 0x30), 0x20045BA5154222DCLL, f, f, 6.0f, packed);
    }

    idx = *(int *)(D_00275C34 + 4);
    *(int *)(D_00275C34 + 4) = idx * 0x25 + 0xB;
    {
        float frac = (float)((idx >> 16) & 0xFFFF) / 65535.0f;
        func_001CFB50(&D_0081F8F0, 0, arg0, *(float *)(D_00275C34 + 0x54),
                      frac + 0.0001f, 1.0f, 1.0000000116860974e-06f, 0.0f);
    }
    func_001CFBE0(arg1, 0, &D_00256EE0, &D_0081F8F0, 0);

    idx = *(int *)(D_00275C34 + 4);
    *(int *)(D_00275C34 + 4) = idx * 0x25 + 0xB;
    {
        float frac = (float)((idx >> 16) & 0xFFFF) / 65535.0f;
        func_001CFB50(&D_0081F8F0, 0, arg0, *(float *)(D_00275C34 + 0x54),
                      frac + 0.0001f, 1.0f, 1.0000000116860974e-06f, 0.0f);
    }
    func_001CFBE0(arg1, 2, &D_00256EE0, &D_0081F8F0, 0);

    idx = *(int *)(D_00275C34 + 4);
    *(int *)(D_00275C34 + 4) = idx * 0x25 + 0xB;
    {
        float frac = (float)((idx >> 16) & 0xFFFF) / 65535.0f;
        func_001CFB50(&D_0081F8F0, 0, arg0, *(float *)(D_00275C34 + 0x54),
                      frac + 0.0001f, 1.0f, 1.0000000116860974e-06f, 0.0f);
    }
    func_001CFBE0(arg1, 1, &D_00256F70, &D_0081F8F0, 0);

    {
        float v = *(float *)(D_00275C34 + 8);
        v = v + ((0.05f - v) / 10.0f);
        *(float *)(D_00275C34 + 8) = v;
        if (v < 0.05f) {
            v = 0.05f;
        }
        *(float *)(D_00275C34 + 8) = v;
    }
}

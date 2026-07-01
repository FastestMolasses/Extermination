// NEARMISS func_001EBF10  (vram 0x001EBF10, 0x2D4 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 56.72% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP-constant emit-order / instruction-scheduling permutation, same wall class already documented on sibling func_001EBD20/func_001EBC30/func_001EC1F0/func_001EC3F0. Logic (three VFX init blocks, LCG jitter, call args, table selection) fully recovered and verified against the splat .s; only the flo...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001029C0(void *);
extern void func_001CFB50(void *, int, void *, float, float, float, float, float);
extern void func_001CFBE0(int, int, void *, void *, int);
extern char *D_00275C34;
extern char D_0081F8F0[0x100];
extern char D_002565E0[0x100];
extern char D_00256670[0x100];
extern float D_700036A0[4];

void func_001EBF10(char *arg0, int arg1) {
    char *dl;
    int v;

    func_001029C0(D_700036A0);

    *(int *)0x700036D0 = 0x43B90000;
    *(float *)0x700036D4 = *(float *)(arg0 + 0x34);
    *(int *)0x700036D8 = 0x43BE0000;
    *(int *)0x700036DC = 0x3F800000;
    dl = D_00275C34;
    v = *(int *)(dl + 4);
    *(int *)(dl + 4) = (v * 0x25) + 0xB;
    func_001CFB50(D_0081F8F0, 0, D_700036A0, *(float *)(dl + 0x54),
                  ((float)((v >> 16) & 0xFFFF) / 65535.0f) + 9.999999974752427e-07f,
                  1.0f, 9.999999974752427e-07f, 0.0f);
    func_001CFBE0(arg1, 1, D_002565E0, D_0081F8F0, 0);

    *(int *)0x700036D0 = 0x43AC8000;
    *(float *)0x700036D4 = *(float *)(arg0 + 0x34);
    *(int *)0x700036D8 = 0x43C30000;
    *(int *)0x700036DC = 0x3F800000;
    dl = D_00275C34;
    v = *(int *)(dl + 4);
    *(int *)(dl + 4) = (v * 0x25) + 0xB;
    func_001CFB50(D_0081F8F0, 0, D_700036A0, *(float *)(dl + 0x54),
                  ((float)((v >> 16) & 0xFFFF) / 65535.0f) + 9.999999974752427e-07f,
                  1.0f, 9.999999974752427e-07f, 0.0f);
    func_001CFBE0(arg1, 1, D_00256670, D_0081F8F0, 0);

    *(int *)0x700036D0 = 0x43C58000;
    *(float *)0x700036D4 = *(float *)(arg0 + 0x34);
    *(int *)0x700036D8 = 0x43C30000;
    *(int *)0x700036DC = 0x3F800000;
    dl = D_00275C34;
    v = *(int *)(dl + 4);
    *(int *)(dl + 4) = (v * 0x25) + 0xB;
    func_001CFB50(D_0081F8F0, 0, D_700036A0, *(float *)(dl + 0x54),
                  ((float)((v >> 16) & 0xFFFF) / 65535.0f) + 9.999999974752427e-07f,
                  1.0f, 9.999999974752427e-07f, 0.0f);
    func_001CFBE0(arg1, 1, D_00256670, D_0081F8F0, 0);
}

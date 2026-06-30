// NEARMISS func_001EF510  (vram 0x001EF510, 0x26C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.63% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// instruction-scheduling permutation: the target reverses the f14(1.0f)/f15(9.99e-7) immediate-load order in ONLY the middle of three structurally-identical effect-spawn blocks. The C body is identical across all three, so a single source cannot emit a per-block scheduler swap. Body fully recovered...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// NEARMISS 98.6% (mwcc 2.3.3; 991202 = 86.9%). Body fully recovered. Spawns
// three particle effects (D_00257990 / D_00257AB0 / D_00257A20), each preceded
// by advancing the LCG counter at D_00275C34[1] and re-arming func_001CFB50 with
// a randomized scalar f = ((counter>>16 & 0xFFFF)/65535 + 1e-4). The first call
// is gated by an init (func_001EFD90) when D_00275C34->0x54 is still 0, and
// func_001EEEB0 runs between blocks 1 and 2. Sole residual: in the MIDDLE of the
// three structurally-identical blocks the target schedules the 1.0f (f14) and
// 9.99e-7 (f15) immediate-loads in the reverse order vs the other two blocks; the
// three blocks are identical C, so one body cannot reproduce a per-block
// scheduler swap. Pure instruction-scheduling permutation -> permuter territory.
extern int *D_00275C34;
extern char *D_00275C30;
extern int D_00257990[2];
extern int D_00257A20[2];
extern int D_00257AB0[2];
extern int D_0081F8F0[2];

void func_001EFD90(int mode, void *p, void *q);
void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);
void func_001EEEB0(int a, int b);

void func_001EF510(int arg0, int arg1) {
    int r;
    float f;

    if (*(float *)((char *)D_00275C34 + 0x54) == 0.0f) {
        func_001EFD90(0x80000036, D_00275C30 + 0xB0, D_00275C30 + 0xC0);
    }

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 6.0f);
    func_001CFBE0(arg1, 1, D_00257990, D_0081F8F0, 0);

    func_001EEEB0(arg0, arg1);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 6.0f);
    func_001CFBE0(arg1, 1, D_00257AB0, D_0081F8F0, 0);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 6.0f);
    func_001CFBE0(arg1, 1, D_00257A20, D_0081F8F0, 0);
}

// NEARMISS func_001EB250  (vram 0x001EB250, 0x1F0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.90% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful.
// Remaining differences: in the FIRST spawn only, the 1.0f (f14) and 15.0f (f16) argument
// materializations are scheduled in swapped order around the counter load (two instruction pairs);
// the second spawn is exact. Scheduler tie-break; permuter candidate.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Spawns two muzzle/impact particle effects for a weapon (arg0) into render
// queue arg1. When the global timer *(D_00275C34+0x54) exceeds 0.8, decays the
// weapon's +0x38 field by 2.0. Calls func_0021B9A0 to set audio/voice params
// (channel 2 -> 0, channel 3 -> 2000.0). For each of the two effects it advances
// the global LCG-style counter at *(D_00275C34+4) (n = n*37 + 11) and feeds
// ((n>>16 & 0xFFFF)/65535 + 0.0001) as a randomized parameter to
// func_001CFB50(template D_0081F8F0, ...) then emits it via func_001CFBE0 with
// effect tables D_00255B30 / D_00255BC0. Finishes by resetting audio channel 1.
//
// Fixes over the 64.54% version (read from the original instructions):
// - D_00275C34 is %gp_rel: -sdatathreshold 4 (siblings func_001EB7F0/001EBA20 use it too).
// - func_001CFBE0 takes 5 arguments (the 5th is 0, in $t0), as its definition in src/ shows.
// - Random-value spelling copied from the byte-matched twins (func_001EB7F0).
// - The two spawns are one inlined helper: its parameter copies are what move the 15.0f
//   and arg0 setup ahead of the counter computation in the original.
// - The final restore call materializes the shared 0.0f in $f13 first (spelling from
//   the byte-matched func_001F5940).
extern int *D_00275C34;
extern int D_00255B30[2];
extern int D_00255BC0[2];
extern int D_0081F8F0[2];

void func_0021B9A0(int mode, float scale, float bias);
void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

static inline void spawn(float f4, int obj, int dst, int kind, void *tbl)
{
    int r;
    float f;

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, obj,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, f4);
    func_001CFBE0(dst, kind, tbl, D_0081F8F0, 0);
}

void func_001EB250(int arg0, int arg1) {
    if (!(*(float *)((char *)D_00275C34 + 0x54) <= 0.8f)) {
        *(float *)(arg0 + 0x38) = *(float *)(arg0 + 0x38) - 2.0f;
    }
    func_0021B9A0(2, 0.0f, 0.0f);
    func_0021B9A0(3, 0.0f, 2000.0f);
    spawn(15.0f, arg0, arg1, 5, D_00255B30);
    spawn(15.0f, arg0, arg1, 5, D_00255BC0);
    { float w = 0.0f; func_0021B9A0(1, (w = 0.0f), w); }
}

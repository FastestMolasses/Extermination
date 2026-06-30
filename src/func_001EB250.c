// NEARMISS func_001EB250  (vram 0x001EB250, 0x1F0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 64.54% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Float-constant materialization order + FP register coloring (scheduling permutation). Body and control flow fully recovered (64.54% with mwcc 2.3.3). The target hoists all the float-constant arg loads (1.0f/1e-6f/15.0f into f14/f15/f16) and the (counter>>16)&0xFFFF int->float conversion ahead of ...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Spawns two muzzle/impact particle effects for a weapon (arg0) into render
// queue arg1. When the global timer *(D_00275C34+0x54) exceeds 0.8, decays the
// weapon's +0x38 field by 2.0. Calls func_0021B9A0 to set audio/voice params
// (channel 2 -> 0, channel 3 -> 2000.0). For each of the two effects it advances
// the global LCG-style counter at *(D_00275C34+4) (n = n*37 + 11) and feeds
// ((n>>16 & 0xFFFF)/65535 + 0.0001) as a randomized parameter to
// func_001CFB50(template &D_0081F8F0, ...) then emits it via func_001CFBE0 with
// effect tables D_00255B30 / D_00255BC0. Finishes by resetting audio channel 1.
//
// NEARMISS 64.5% (mwcc 2.3.3) / 59.4% (mwcc 991202). Logic fully recovered;
// residual is float-constant materialization order + FP register coloring
// (the target hoists the const args and the int->float conversion ahead of the
// counter store). Not the clean-store nop. Permuter (FP-coloring/scheduling).

extern char *D_00275C34;
extern char D_00255B30;
extern char D_00255BC0;
extern char D_0081F8F0;

void func_0021B9A0(int, float, float);
void func_001CFB50(char *, int, char *, float, float, float, float, float);
void func_001CFBE0(int, int, char *, char *);

void func_001EB250(char *arg0, int arg1) {
    int n;

    if (!(*(float *)(D_00275C34 + 0x54) <= 0.8f)) {
        *(float *)(arg0 + 0x38) = *(float *)(arg0 + 0x38) - 2.0f;
    }
    func_0021B9A0(2, 0.0f, 0.0f);
    func_0021B9A0(3, 0.0f, 2000.0f);

    n = *(int *)(D_00275C34 + 4);
    *(int *)(D_00275C34 + 4) = n * 37 + 0xB;
    func_001CFB50(&D_0081F8F0, 0, arg0, *(float *)(D_00275C34 + 0x54),
                  (float)((n >> 0x10) & 0xFFFF) / 65535.0f + 0.0001f,
                  1.0f, 0.0000010f, 15.0f);
    func_001CFBE0(arg1, 5, &D_00255B30, &D_0081F8F0);

    n = *(int *)(D_00275C34 + 4);
    *(int *)(D_00275C34 + 4) = n * 37 + 0xB;
    func_001CFB50(&D_0081F8F0, 0, arg0, *(float *)(D_00275C34 + 0x54),
                  (float)((n >> 0x10) & 0xFFFF) / 65535.0f + 0.0001f,
                  1.0f, 0.0000010f, 15.0f);
    func_001CFBE0(arg1, 5, &D_00255BC0, &D_0081F8F0);

    func_0021B9A0(1, 0.0f, 0.0f);
}

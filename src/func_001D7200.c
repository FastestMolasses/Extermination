// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Appends a clamped 2D coordinate to the per-frame screen-point list. The list
// lives at D_008172C0, banked by the current state record's slot index
// (D_00275670[0x9C] * 0x480) and indexed by the running counter D_00275C08
// (0x48-byte stride). Returns -1 when the list is full (counter >= 0x10).
// arg0 (x) is clamped to [0, 0x1FF] via a float (< 512.0) compare; arg1 (y) is
// clamped to [0, 0xDF]. Both are written as floats at +0x0 and +0x4. On
// success bumps the counter and returns the pre-increment slot index.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202: under
// 991202 the lone residual is the CW-vs-mwcc branch lowering of the early
// `return -1` block (991202 reorders the b/nop and uses $at for the slti). The
// 2.3.3 build reproduces CW's lowering exactly. Verified objdiff 100.0% with
// relocation tables identical vs build/expected/func_001D7200.o.
extern char *D_00275670;
extern int D_00275C08;
extern char D_008172C0[0x100000];

int func_001D7200(int a0, int a1)
{
    int v0;
    char *v1;

    if (D_00275C08 >= 0x10) {
        return -1;
    }
    v1 = D_008172C0 + (*((int *)(D_00275670 + 0x9C)) * 0x480) + (D_00275C08 * 0x48);
    if (a0 < 0) {
        a0 = 0;
    }
    if (!((float)a0 < 512.0f)) {
        a0 = 0x1FF;
    }
    if (a1 < 0) {
        a1 = 0;
    }
    if (!(a1 < 0xE0)) {
        a1 = 0xDF;
    }
    *((float *)(v1 + 0)) = (float)a0;
    *((float *)(v1 + 4)) = (float)a1;
    v0 = D_00275C08;
    D_00275C08 = v0 + 1;
    return v0;
}

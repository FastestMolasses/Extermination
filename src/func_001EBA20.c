// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Fires two identical particle/effect spawn passes off the gp-relative RNG
// state record *D_00275C34. Each pass: read the 32-bit LCG seed at +0x04,
// turn its high 16 bits into a [0,1) fraction via /65535.0f + 0.0001f bias,
// advance the seed (seed = seed*0x25 + 0xB), then call func_001CFB50 with the
// shared sprite/anim block D_0081F8F0, the fade scalar at *(D_00275C34+0x54),
// that random fraction, and the unit/epsilon/3.0 tuning constants; finally
// queue the result via func_001CFBE0 against a per-pass descriptor
// (D_002560D0 then D_00256160). D_00275C34 is gp-relative (sdatathreshold 4);
// D_0081F8F0 / D_002560D0 / D_00256160 stay absolute. Same idiom family as the
// matched siblings func_001EB7F0 / func_001EACF0.
//
// Matched 100.0 with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build
// reaches 89.3% (it fills the clean div.s/store delay slots the CW target
// leaves). Verified objdiff 100.0 vs build/expected/func_001EBA20.o.
extern int *D_00275C34;
extern int D_002560D0[2];
extern int D_00256160[2];
extern int D_0081F8F0[2];

void func_001CFB50(void *p, int b, int c, float f0, float f1, float f2, float f3, float f4);
void func_001CFBE0(int a, int b, void *c, void *d, int e);

void func_001EBA20(int arg0, int arg1) {
    int r;
    float f;

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 3.0f);
    func_001CFBE0(arg1, 1, D_002560D0, D_0081F8F0, 0);

    r = D_00275C34[1];
    f = (float)((r >> 0x10) & 0xFFFF);
    f /= 65535.0f;
    D_00275C34[1] = r * 0x25 + 0xB;
    f += 0.0001f;
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  f, 1.0f, 9.999999974752427e-07f, 3.0f);
    func_001CFBE0(arg1, 1, D_00256160, D_0081F8F0, 0);
}

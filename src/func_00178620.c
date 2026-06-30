// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Loads a hardcoded set of clip/visibility-frustum floats into the scratchpad
// at 0x700038A0..0x700038BC (two parameter blocks A=+0xA0, B=+0xB0), choosing
// the near/far set by arg1 (arg1==0 -> the -9/-6 set, else the +9 set). Both
// blocks share the same +0xA4/+0xA8/+0xAC/+0xB8/+0xBC values. It then projects
// the entity transform (arg0+0xD0) through each block via func_001026A0 into
// scratch vectors C(0x700038C0)/D(0x700038D0), tests them with func_0019AFE0
// (mask &6). On a hit it reads the active-camera/mode byte at *(0x700031D0)+0x1A:
// 0x3D -> func_00178390 (returns 3 on success); 0x3B -> func_001782A0 (sets the
// arg0+0xD flag and returns 1 on success). Returns the resulting status.
// objdiff 100.0 vs expected on mwcc 2.3.3 (mwcps2-2.3.3-000906); pinned 991202
// reaches 86.9%. Key: D_700038xx and the 0x700031D0 indirection are raw
// hardcoded scratchpad addresses (cast-literal pointers), NOT relocated symbols,
// so the loads emit plain lui 0x7000 / lw 0x31d0(at) with no relocation.
extern void func_001026A0(float *dst, float *src, float *m);
extern int func_0019AFE0(char *p, float *a, float *b, int n);
extern int func_00178390(char *p);
extern int func_001782A0(char *p);
extern float D_700038A0[4];
extern float D_700038B0[4];
extern float D_700038C0[4];
extern float D_700038D0[4];

int func_00178620(char *arg0, int arg1) {
    int r;

    r = 0;
    if (arg1 == 0) {
        *(int *)0x700038A0 = 0xC1100000;
        *(int *)0x700038B0 = 0xC1100000;
        *(int *)0x700038A4 = 0x419C0000;
        *(int *)0x700038B4 = 0x419C0000;
        *(int *)0x700038A8 = 0xC0C00000;
        *(int *)0x700038AC = 0x3F800000;
        *(int *)0x700038B8 = 0x40800000;
        *(int *)0x700038BC = 0x3F800000;
    } else {
        *(int *)0x700038A0 = 0x41100000;
        *(int *)0x700038B0 = 0x41100000;
        *(int *)0x700038A4 = 0x419C0000;
        *(int *)0x700038B4 = 0x419C0000;
        *(int *)0x700038A8 = 0xC0C00000;
        *(int *)0x700038AC = 0x3F800000;
        *(int *)0x700038B8 = 0x40800000;
        *(int *)0x700038BC = 0x3F800000;
    }
    func_001026A0(D_700038C0, (float *)(arg0 + 0xD0), D_700038A0);
    func_001026A0(D_700038D0, (float *)(arg0 + 0xD0), D_700038B0);
    if (func_0019AFE0(arg0, D_700038C0, D_700038D0, 7) & 6) {
        unsigned char st = *(unsigned char *)(*(char **)0x700031D0 + 0x1A);
        if (st == 0x3D) {
            if (func_00178390(arg0) != 0) {
                return 3;
            }
        } else if (st == 0x3B) {
            if (func_001782A0(arg0) != 0) {
                r = 1;
                *(char *)(arg0 + 0xD) = 1;
            }
        }
    }
    return r;
}

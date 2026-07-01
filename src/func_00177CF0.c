// NEARMISS func_00177CF0  (vram 0x00177CF0, 0x250 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.71% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// FP register-coloring + arg-move-order permutation (f0/f1 swap on the local-vector setup and the func_00102918 a1/a2 order). Body/structure byte-identical; 99.71% on mwcc 2.3.3. Permuter (FP-coloring) territory.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void func_001029C0(void *a);
extern void func_00102BB0(void *a, void *b, float f);
extern void func_00102918(void *a, void *b, float *c);
extern void func_001026A0(void *a, void *b, void *c);
extern int func_0019AFE0(int obj, void *a, void *b, int n);
extern char D_700036A0[];
extern char D_700038A0[];
extern char D_700038B0[];
extern char D_700038C0[];
extern char D_700038D0[];

struct vec4 { float x; float y; float z; int w; };

int func_00177CF0(int arg0, float fparg0) {
    struct vec4 sp20;
    int r;

    float a = *(float *)0x70003050;
    float b = *(float *)0x70003058;
    sp20.x = a;
    sp20.y = fparg0;
    sp20.z = b;
    sp20.w = 0x3F800000;
    func_001029C0(D_700036A0);
    func_00102BB0(D_700036A0, D_700036A0, *(float *)0x700031E4);
    func_00102918(D_700036A0, D_700036A0, (float *)&sp20);
    *(int *)0x700038A0 = 0xC0900000;
    *(int *)0x700038B0 = 0xC0900000;
    *(int *)0x700038A4 = 0;
    *(int *)0x700038B4 = 0;
    *(int *)0x700038A8 = 0xC0000000;
    *(int *)0x700038AC = 0x3F800000;
    *(int *)0x700038B8 = 0x40000000;
    *(int *)0x700038BC = 0x3F800000;
    func_001026A0(D_700038C0, D_700036A0, D_700038A0);
    func_001026A0(D_700038D0, D_700036A0, D_700038B0);
    r = func_0019AFE0(arg0, D_700038C0, D_700038D0, 7);
    if (r & 1) {
        return 1;
    }
    if (r & 6) {
        if (*(unsigned char *)(*(int *)0x700031D0 + 0x1A) == 0x32) {
            return 1;
        }
    }
    *(int *)0x700038A0 = 0x40900000;
    *(int *)0x700038B0 = 0x40900000;
    *(int *)0x700038A4 = 0;
    *(int *)0x700038B4 = 0;
    *(int *)0x700038A8 = 0xC0000000;
    *(int *)0x700038AC = 0x3F800000;
    *(int *)0x700038B8 = 0x40000000;
    *(int *)0x700038BC = 0x3F800000;
    func_001026A0(D_700038C0, D_700036A0, D_700038A0);
    func_001026A0(D_700038D0, D_700036A0, D_700038B0);
    r = func_0019AFE0(arg0, D_700038C0, D_700038D0, 7);
    if (r & 1) {
        return 1;
    }
    if (r & 6) {
        if (*(unsigned char *)(*(int *)0x700031D0 + 0x1A) == 0x32) {
            return 1;
        }
    }
    return 0;
}

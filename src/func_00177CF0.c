// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SPAD: 0x70003058
//
// Sibling of func_00177F40: sp20 = (*0x70003050, fparg0, *0x70003058, 1.0f)
// goes to func_00102918 after func_001029C0 / func_00102BB0 (angle at
// 0x700031E4) on D_700036A0. Two rounds then put the point pairs
// (-4.5, 0, -2, 1) / (-4.5, 0, 2, 1) at 0x700038A0 / 0x700038B0 (the second
// round with x = +4.5), transform them with func_001026A0 into 0x700038C0 /
// 0x700038D0 and call func_0019AFE0(arg0, C0, D0, 7). A result with bit 0, or
// with bits 1-2 while the byte at (*0x700031D0)+0x1A is 0x32, returns 1;
// otherwise 0.
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 6, 2026-10-20); it was
// a 99.71 NEARMISS. Keys: 0x70003058 is a relocated extern (MATCHING_GUIDE
// idiom-32, // SPAD:), and func_00102918's third parameter is `void *` as in
// its own definition: the old `(float *)&sp20` cast added weight, so mwcc set
// a2 before a1 (MATCHING_GUIDE, argument evaluation order).

extern float D_70003058;
extern void func_001029C0(void *a);
extern void func_00102BB0(void *a, void *b, float f);
extern void func_00102918(void *a, void *b, void *c);
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
    float b = D_70003058;
    sp20.x = a;
    sp20.y = fparg0;
    sp20.z = b;
    sp20.w = 0x3F800000;
    func_001029C0(D_700036A0);
    func_00102BB0(D_700036A0, D_700036A0, *(float *)0x700031E4);
    func_00102918(D_700036A0, D_700036A0, &sp20);
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

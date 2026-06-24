// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-frame setup of a display/animation record at D_008101E0 from the current
// scene table entry. Indexes a nested pointer table D_0024D650[idx0][idx1]
// (idx0=D_00810700, idx1=D_00810701), advances by D_00810702*0x30 to the active
// element `p`, mirrors its float at p+0x18 into D_00810244 and D_008101EC, and
// clears the flag byte D_008101E7. Byte p+0x10's high bit becomes the enable
// flag at base[5]; its low 7 bits go to base[6]. When enabled, copies a 3-float
// row from D_0024A8D0[(p+0x10 >> 8)*0xC], sets base[0x1C]=1.0f (0x3F800000),
// composes two transforms via func_00102948, biases base[0x24] by +15.0f, then
// finalizes via func_001DD980(D_008105D0, D_008105E0).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): objdiff 100.0 byte-identical vs
// build/expected/func_001B0300.o (function's 344 code bytes verified identical;
// trailing 8 object bytes are section-alignment zero padding). The pinned
// 991202 build reaches only 90.98% (clean-store delay-slot-nop / branch
// lowering it cannot reproduce). func_00102948 is left unprototyped on purpose:
// it is called with 3 args once and 2 args twice, matching the original's
// argument setup exactly (a prototype would force mwcc to zero a2).
extern void func_00102948();
extern void func_001DD980(float *a, float *b);

extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;
extern unsigned char D_008101E0;
extern unsigned char D_008101E7;
extern float D_008101EC;
extern float D_00810244;
extern float D_00810350;
extern float D_008105D0;
extern float D_008105E0;
extern int **D_0024D650[];
extern float D_0024A8D0;

void func_001B0300(void) {
    unsigned char *p;
    unsigned char *base;
    int v1;
    float f;

    base = &D_008101E0;
    p = (unsigned char *)((int *)D_0024D650[D_00810700][D_00810701]);
    p += D_00810702 * 0x30;
    D_008101E7 = 0;
    f = *(float *)(p + 0x18);
    D_00810244 = f;
    D_008101EC = f;
    v1 = *(unsigned char *)(p + 0x10);
    if (v1 & 0x80) {
        *(base + 5) = 1;
    } else {
        *(base + 5) = 0;
    }
    *(base + 6) = (unsigned char)(v1 & 0x7F);
    if (*(base + 5) == 1) {
        float *m = (float *)((char *)&D_0024A8D0 + (*(int *)(p + 0x10) >> 8) * 0xC);
        *(float *)(base + 0x10) = m[0];
        *(float *)(base + 0x14) = m[1];
        *(float *)(base + 0x18) = m[2];
        *(int *)(base + 0x1C) = 0x3F800000;
        func_00102948((float *)(base + 0x20), &D_00810350, m);
        {
            float c = 15.0f;
            *(float *)(base + 0x24) = *(float *)(base + 0x24) + c;
        }
        func_00102948(&D_008105E0, (float *)(base + 0x20));
        func_00102948(&D_008105D0, (float *)(base + 0x10));
    }
    func_001DD980(&D_008105D0, &D_008105E0);
}

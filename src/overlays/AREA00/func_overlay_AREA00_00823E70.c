// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x00823EB0 (splat/link name 00823E70; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: state 1 copies the +0xD0 matrix, moves its translation row to
//  (0, 0, 1, 1) through it, sets the 0x8289B0 scale to 12 + 12 * max(-dot, 0)
//  (D_70003A20), submits a packet (func_001CFA60 / func_001CFBE0, table
//  0x828990) and, when func_001C6190(self) is outside 320..440, draws a
//  func_001CD520 sprite tinted by the colour at 0x828A20 times (128 - rand % 32).
// Matching: the colour word is accumulated in one local
//  (c = (b >> 7) << 16; c |= (g >> 7) << 8; c |= r >> 7), and the 0.1 argument
//  of func_001CFA60 is staged from a block-local integer (idiom-31) so its
//  constant is materialized before the 1.0.
typedef struct { unsigned int c[4]; } Rgba __attribute__((aligned(16)));
extern Rgba D_overlay_AREA00_00828A20;
extern char D_overlay_AREA00_00828990[];
extern float D_overlay_AREA00_008289B0[4];
extern char D_700038A0[];
extern float D_00810600;
extern void func_00102958(void *dst, void *src);
extern void func_001026A0(void *a, void *b, void *c);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_00102760(void *a, void *b);
extern float func_00102738(void *a, void *b);
extern int func_001CCF70(void *p);
extern void func_001CFA60(void *sp, void *pos, float f12, float f13);
extern void func_001CFBE0(int a0, int a1, void *a2, void *a3, int t0);
extern int func_001C6190(unsigned char *self);
extern int func_00122BB8(void);
extern int func_001CD520(int bucket, int mode, void *world, unsigned long long giftag,
                         float w, float h, float zbias, unsigned int rgba);

void func_overlay_AREA00_00823E70(unsigned char *self) {
    float m[4][4];
    int pkt[24];
    Rgba col = D_overlay_AREA00_00828A20;
    float *pos;
    float d;
    float s;
    int h;
    float x;
    unsigned int k;
    unsigned int r, g, b;
    unsigned int c;
    switch (self[4]) {
    case 0:
        break;
    case 1:
        func_00102958(m, self + 0xD0);
        pos = m[3];
        pos[0] = 0.0f;
        m[3][1] = 0.0f;
        m[3][2] = 1.0f;
        m[3][3] = 1.0f;
        func_001026A0(pos, self + 0xD0, pos);
        func_001026A0(D_700038A0, m, D_overlay_AREA00_00828990);
        func_001028D0(D_700038A0, D_700038A0, pos);
        func_00102760(D_700038A0, D_700038A0);
        d = -func_00102738(D_700038A0, &D_00810600);
        *(volatile float *)0x70003A20 = d;
        if (d < 0.0f) {
            d = 0.0f;
        }
        *(volatile float *)0x70003A20 = d;
        s = 12.0f + 12.0f * d;
        D_overlay_AREA00_008289B0[0] = s;
        D_overlay_AREA00_008289B0[1] = s;
        D_overlay_AREA00_008289B0[2] = s;
        D_overlay_AREA00_008289B0[3] = 0.0f;
        h = func_001CCF70(self + 0x100);
        {
            int one = 1;
            float arg = (float)one * 0.1f;
            func_001CFA60(pkt, m, 1.0f, arg);
        }
        func_001CFBE0(h, 1, D_overlay_AREA00_00828990, pkt, 1);
        x = (float)func_001C6190(self);
        if (x < 320.0f || x > 440.0f) {
            k = 0x80 - (func_00122BB8() & 0x1F);
            b = col.c[2] * k;
            g = col.c[1] * k;
            r = col.c[0] * k;
            c = (b >> 7) << 16;
            c |= (g >> 7) << 8;
            c |= r >> 7;
            func_001CD520(0, 2, pos, 0x20045B0599421EF0ULL, 7.0f, 7.0f, 0.0f, c);
        }
        break;
    case 2:
    case 3:
        break;
    }
}

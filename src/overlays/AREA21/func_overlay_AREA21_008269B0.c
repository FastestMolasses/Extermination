// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x008269F0 (splat/link name 008269B0; overlay code
//  is linked 0x40 below where it runs), 0x1BC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: script callback (script 0x82BEC0, op09 record 0x82C340), called
//  with (self, record, work). Record +4 0: saves D_008105D0 / D_008105E0 to
//  work +0x20 / +0x30, work +0x10 = 0. While work +0x10 < work +0xC: +1 a
//  frame, and on odd func_001281C0 values D_008105D0 / D_008105E0 become
//  the saved vectors plus a func_001026A0(D_00810650, (t1 / 10, t2 / 10, 0,
//  1)) offset, t1 / t2 from the 32-float tables 0x82CA00 / 0x82CA80; at the
//  end the saved vectors are restored and it returns 1.
#define SPF(a) (*(float *)(a))
extern float D_008105D0[4];
extern float D_008105E0[4];
extern float D_00810650[];
extern float D_70003600[4];
extern float D_70003610[4];
extern float D_overlay_AREA21_0082CA00[32];
extern float D_overlay_AREA21_0082CA80[32];
extern void func_00102948(void *dst, void *src);
extern int func_001281C0(float f);
extern void func_001026A0(void *dst, void *a, void *b);
extern void func_001028B8(void *dst, void *a, void *b);

int func_overlay_AREA21_008269B0(unsigned char *self, unsigned char *rec, unsigned char *w) {
    float t;
    switch (rec[4]) {
    case 0:
        func_00102948(w + 0x20, D_008105D0);
        func_00102948(w + 0x30, D_008105E0);
        rec[4]++;
        *(float *)(w + 0x10) = 0.0f;
    case 1:
        if (!(*(float *)(w + 0x10) < *(float *)(w + 0xC))) {
            func_00102948(D_008105D0, w + 0x20);
            func_00102948(D_008105E0, w + 0x30);
            return 1;
        }
        *(float *)(w + 0x10) += 1.0f;
        t = *(float *)(w + 0x10);
        if (func_001281C0(t) & 1) {
            SPF(0x70003600) = D_overlay_AREA21_0082CA00[func_001281C0(t) & 0x1F] / 10.0f;
            SPF(0x70003604) = D_overlay_AREA21_0082CA80[func_001281C0(*(float *)(w + 0x10)) & 0x1F] / 10.0f;
            SPF(0x70003608) = 0.0f;
            SPF(0x7000360C) = 1.0f;
            func_001026A0(D_70003610, D_00810650, D_70003600);
            func_001028B8(D_008105D0, w + 0x20, D_70003610);
            func_001028B8(D_008105E0, w + 0x30, D_70003610);
        }
        break;
    }
    return 0;
}

// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00829F40 (splat/link name 00829F00; overlay code
//  is linked 0x40 below where it runs), 0x28C bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: called from 0x8297C0 while +5 is 2. Every third D_70003B68 frame
//  with an odd func_00122BB8(), effect 0x8000003B (func_001EFEB0) at the
//  +0x114 model's +0xC0 rotated by pi/2 and offset (+-2.5, 13, +-2.5), its
//  +5 = 1, +0x1F0 = 8, +0x1F4 = 12, +0x1F8 = 0.2. Each call queues a
//  func_001CFB50 packet (group 0x82DB20) on the +0x118 model with +0x240
//  (wrapping above 2.0) and +0x22C / 1800.
extern int D_70003B68[];
extern char D_overlay_AREA21_0082DB20[];
extern int func_00122BB8(void);
extern void func_001029C0(void *m);
extern void func_00102B08(void *dst, void *src, float a);
extern void func_00102918(void *a, void *b, void *c);
extern unsigned char *func_001EFEB0(int id, void *m);
extern void func_00102958(void *dst, void *src);
extern int func_001CCF70(void *p);
extern void func_001CFB50(void *p, int a1, void *a2, float f12, float f13, float f14, float f15, float f16);
extern void func_001CFBE0(int h, int a1, void *a2, void *a3, int t0);

void func_overlay_AREA21_00829F00(unsigned char *self) {
    float m[16];
    int pkt[24];
    unsigned char *o;
    int seed;
    float t;
    float r;
    int h;
    unsigned char *w = self + 0x1F0;
    if ((func_00122BB8() & 1) && D_70003B68[0] % 3 == 0) {
        func_001029C0(m);
        func_00102B08(m, m, 1.5707964f);
        func_00102918(m, m, *(unsigned char **)(self + 0x114) + 0xC0);
        m[13] += 13.0f;
        m[12] += 5.0f * ((float)func_00122BB8() / 2147483648.0f) - 2.5f;
        m[14] += 5.0f * ((float)func_00122BB8() / 2147483648.0f) - 2.5f;
        o = func_001EFEB0(0x8000003B, m);
        if (o != 0) {
            o[5] = 1;
            *(int *)(o + 0x1F0) = 8;
            *(float *)(o + 0x1F4) = 12.0f;
            *(float *)(o + 0x1F8) = 0.2f;
        }
    }
    func_00102958(m, *(unsigned char **)(self + 0x118) + 0x90);
    m[13] += 1.0f;
    seed = *(int *)(w + 0x54);
    t = *(float *)(w + 0x3C) / 1800.0f;
    h = func_001CCF70(&m[12]);
    r = (float)((seed >> 16) & 0xFFFF);
    r = r / 65535.0f;
    r += 0.0001f;
    { int k = 7; float s = (float)k; func_001CFB50(pkt, 0, m, *(float *)(w + 0x50), r, t, 0.1f, s); }
    func_001CFBE0(h, 1, D_overlay_AREA21_0082DB20, pkt, 0);
    *(float *)(w + 0x50) += 0.01f;
    if (!(*(float *)(w + 0x50) <= 2.0f)) {
        *(float *)(w + 0x50) -= 1.0f;
    }
}

// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00828410 (splat/link name 008283D0; overlay code
//  is linked 0x40 below where it runs), 0x2F0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Covers the splat pieces 008283D0, 00828410 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: called from 0x828700 with (self, work). With the +0x118 model's
//  +0x7C..84 zeroed for the duration: func_001C6380, then two points (-1.5 /
//  1.5, 3.6, -24) through the model matrix into work +0x10 / +0x20
//  (func_001031E0), then the 0x829590 steps (work, +0x70, D_008101F0 /
//  D_008105D0, D_00810200 / D_008105E0).
#define SPF(a) (*(float *)(a))
#define MDL (*(unsigned char **)(self + 0x118))
extern float D_70003600[4];
extern float D_70003610[4];
extern float D_008101F0[4];
extern float D_00810200[4];
extern float D_008105D0[4];
extern float D_008105E0[4];
extern void func_001C6380(unsigned char *self);
extern void func_001026A0(void *dst, void *a, void *b);
extern void func_001031E0(void *dst, void *src);
extern void func_001028D0(void *dst, void *a, void *b);
extern void func_00102760(void *dst, void *src);
extern void func_00102948(void *dst, void *src);

void func_overlay_AREA21_008283D0(unsigned char *self, unsigned char *out) {
    float x = *(float *)(MDL + 0x7C);
    float y = *(float *)(MDL + 0x80);
    float z = *(float *)(MDL + 0x84);
    *(float *)(MDL + 0x7C) = 0.0f;
    *(float *)(MDL + 0x80) = 0.0f;
    *(float *)(MDL + 0x84) = 0.0f;
    func_001C6380(self);
    SPF(0x70003610) = -1.5f;
    SPF(0x70003614) = 3.6f;
    SPF(0x70003618) = -24.0f;
    SPF(0x7000361C) = 1.0f;
    func_001026A0(D_70003610, MDL + 0x90, D_70003610);
    func_001031E0(out + 0x10, D_70003610);
    SPF(0x70003610) = 1.5f;
    SPF(0x70003614) = 3.6f;
    SPF(0x70003618) = -24.0f;
    SPF(0x7000361C) = 1.0f;
    func_001026A0(D_70003610, MDL + 0x90, D_70003610);
    func_001031E0(out + 0x20, D_70003610);
    SPF(0x70003600) = 0.0f;
    SPF(0x70003604) = 4.0f;
    SPF(0x70003608) = 0.0f;
    SPF(0x7000360C) = 1.0f;
    func_001026A0(D_70003600, MDL + 0x90, D_70003600);
    SPF(0x70003610) = 0.0f;
    SPF(0x70003614) = 4.0f;
    SPF(0x70003618) = -8.2f;
    SPF(0x7000361C) = 1.0f;
    func_001026A0(D_70003610, MDL + 0x90, D_70003610);
    func_001031E0(out, D_70003610);
    func_001028D0(D_70003600, D_70003610, D_70003600);
    func_00102760(self + 0x70, D_70003600);
    SPF(0x70003600) = 0.0f;
    SPF(0x70003604) = 6.5f;
    SPF(0x70003608) = 5.0f;
    SPF(0x7000360C) = 1.0f;
    func_001026A0(D_70003600, MDL + 0x90, D_70003600);
    func_001031E0(D_008101F0, D_70003600);
    func_001031E0(D_008105D0, D_008101F0);
    SPF(0x70003610) = 0.0f;
    SPF(0x70003614) = 5.0f;
    SPF(0x70003618) = -8.0f;
    SPF(0x7000361C) = 1.0f;
    func_001026A0(D_70003610, MDL + 0x90, D_70003610);
    func_00102948(D_00810200, D_70003610);
    func_001031E0(D_008105E0, D_00810200);
    *(float *)(MDL + 0x7C) = x;
    *(float *)(MDL + 0x80) = y;
    *(float *)(MDL + 0x84) = z;
}

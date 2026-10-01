// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00823580 (splat/link name 00823540; overlay code
//  is linked 0x40 below where it runs), 0x224 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Role: group 0x82B500 member (script 0x82B5E0's word; x2). +0x1F0 counts
//  consecutive frames (reset when +0x1F4 + 1 != D_70003B68), +0x1F4 =
//  D_70003B68. By +0x94: 3: k = 20 (35 for 500 < func_001C6190(self) < 600, 15
//  for 1040 < v < 1080); when v <= 1000 or v >= 1040 and the count is a
//  multiple of k, effect 0x80000059 at (+0x100, 15, +0x108 + 10 *
//  func_0011DE90(2pi * rand / 2^31 - pi)) via func_001EFD20. 5: every 12th
//  frame func_00122BB8() and effect 0x80000047 at (+0x100, 15, 15 +
//  +0x108).
typedef struct {
    int count;
    int last;
} Work;
typedef struct { float x, y, z; } Vec3;
extern int D_70003B68[];
extern int func_001C6190(unsigned char *self);
extern int func_00122BB8(void);
extern float func_0011DE90(float a);
extern void func_001EFD20(int id, void *pos);

void overlay_AREA21_func_00823540(unsigned char *self) {
    Work *w = (Work *)(self + 0x1F0);
    Vec3 pos;
    float r;
    int v;
    int k;
    switch (self[4]) {
    case 0:
        w->count = 0;
        w->last = D_70003B68[0];
        break;
    case 1:
        if (w->last + 1 != D_70003B68[0]) {
            w->count = 0;
        } else {
            w->count++;
        }
        w->last = D_70003B68[0];
        switch (*(short *)(self + 0x94)) {
        case 3:
            v = func_001C6190(self);
            k = 20;
            if (v > 500 && v < 600) {
                k = 35;
            }
            if (v > 1040 && v < 1080) {
                k = 15;
            }
            if ((v <= 1000 || v >= 1040) && w->count % k == 0) {
                r = 6.2831855f * ((float)func_00122BB8() / 2147483648.0f) - 3.1415927f;
                pos.y = 15.0f;
                pos.x = *(float *)(self + 0x100);
                pos.z = *(float *)(self + 0x108) + 10.0f * func_0011DE90(r);
                func_001EFD20(0x80000059, &pos);
            }
            break;
        case 5:
            if (w->count % 12 == 0) {
                func_00122BB8();
                pos.y = 15.0f;
                pos.x = *(float *)(self + 0x100);
                pos.z = 15.0f + *(float *)(self + 0x108);
                func_001EFD20(0x80000047, &pos);
            }
            break;
        }
        break;
    case 2:
    case 3:
        break;
    }
}

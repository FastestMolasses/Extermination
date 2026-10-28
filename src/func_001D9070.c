// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 96.07% NEARMISS).
// Fixes read from the original instructions:
//  - Locals declared in the order loop counters, group pointer, element pointers,
//    result pointer, value (fixes the integer register colouring).
//  - One float variable carries the scaled value through the clamp, and the four
//    stores are one chained assignment of 2*v (fixes the float register colouring).
//  - The group pointer advances in the outer for-increment after the counter.
//
// For every group in the table returned by func_001C6120(D_0028A56C, 0x16), walks its
// 0x20 elements: divides the element's float at offset 0x48 by 190, maps it to an intensity
// (0 beyond 0.3, 1 below 0, else 1 - t clamped to [0,1]), doubles it and writes it
// to four consecutive float fields of the element.

extern char *func_001C6120(char *a, int b);
extern int D_0028A56C;

void func_001D9070(void) {
    int i;
    int j;
    char *grp;
    char *st;
    char *ld;
    int *p;
    float v;

    p = (int *)func_001C6120((char *)D_0028A56C, 0x16);
    grp = (char *)p + 0x40;
    for (i = 0; i < *p; i += 1, grp += 0x820) {
        st = grp + 0x10;
        ld = st;
        for (j = 0; j < 0x20; j += 1) {
            v = *(float *)(ld + 0x38) / 190.0f;
            if (!(v <= 0.3f)) {
                v = 0.0f;
            } else if (v < 0.0f) {
                v = 1.0f;
            } else {
                v = 1.0f - v;
                if (v < 0.0f) {
                    v = 0.0f;
                }
                if (!(v <= 1.0f)) {
                    v = 1.0f;
                }
            }
            *(float *)(st + 0x20) = *(float *)(st + 0x24) = *(float *)(st + 0x28) = *(float *)(st + 0x2C) = 2.0f * v;
            ld += 0x40;
            st += 0x40;
        }
    }
}

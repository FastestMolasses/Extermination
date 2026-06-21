// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Copies a 4-float vector (struct+0xB0/B4/B8, w=1.0f) into the absolute
// scratch region 0x700038A0, then func_00102948 (16-byte lq/sq copy)
// duplicates it into D_008105E0. Double indirection: a0->0x1C->0x1C.
// The 0x70003xxx stores are absolute-addressed (lui 0x7000); the call
// argument materializes &D_700038A0 as a relocated symbol. mwcc233
// schedules the lui a2,0x3F80 / lui %hi(D_008105E0) pair as the target does
// (the pinned 991202 build orders them differently).
extern char D_008105E0[0x100];
extern float D_700038A0;
extern void func_00102948(void *dst, void *src);

int func_00158050(int a0) {
    int p = *(int *)(*(int *)(a0 + 0x1C) + 0x1C);
    *(float *)0x700038A0 = *(float *)(p + 0xB0);
    *(float *)0x700038A4 = *(float *)(p + 0xB4);
    *(float *)0x700038A8 = *(float *)(p + 0xB8);
    *(float *)0x700038AC = 1.0f;
    func_00102948(D_008105E0, &D_700038A0);
    return 1;
}

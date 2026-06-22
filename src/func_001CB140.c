// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// D_00275670 holds a pointer to a render/display-list context struct; its
// field at +0x1C is the current display-list write cursor (a pointer). This
// function brackets work with func_001D8C20(5)/func_001D8C20(0) (a lock or
// DMA-state toggle writing D_00275670+0x246C), submits two draw setups
// (func_001C7420 with selector 0x3F5/3, and func_001D4430 tail-thunk -> sel 3),
// then emits a 0x10-byte command into the display list (tag byte 0x60 at +3,
// clears the int at +4 and the short at +0) and advances the cursor by 0x10.
// Finally it issues func_001CAAC0(D_00275B44+0xB0, saved_cursor).
extern char *D_00275670;
extern char *D_00275B44;

extern void func_001D8C20(int a0);
extern void func_001C7420(int a0, int a1, int a2);
extern void func_001D4430(int a0, int a1);
extern void func_001CAAC0(int a0, int a1);

void func_001CB140(int a0, int a1) {
    char *base;
    char *p;

    p = *(char **)(D_00275670 + 0x1C);
    func_001D8C20(5);
    func_001C7420(a0, 0x3F5, 3);
    func_001D4430(a1, a0 + 0x80);

    base = D_00275670;
    *(char *)(*(char **)(base + 0x1C) + 3) = 0x60;
    *(int *)(*(char **)(base + 0x1C) + 4) = 0;
    *(short *)(*(char **)(base + 0x1C) + 0) = 0;
    *(char **)(base + 0x1C) = *(char **)(base + 0x1C) + 0x10;

    func_001CAAC0((int)(D_00275B44 + 0xB0), (int)p);
    func_001D8C20(0);
}

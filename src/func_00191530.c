// CFLAGS: -O4,p -sdatathreshold 0
extern void func_00102948(int, int);
extern float D_008105E0;

void func_00191530(int a0, int a1) {
    float f1;
    float f0;
    func_00102948(a0 + 0x20, a1 + 0xA0);
    f1 = *(float *)((char *)a0 + 0x24);
    f0 = 17.0f;
    f0 = f1 + f0;
    *(float *)((char *)a0 + 0x24) = f0;
    func_00102948((int)&D_008105E0, a0 + 0x20);
}

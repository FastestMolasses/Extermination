// CFLAGS: -O4,p -sdatathreshold 4
extern void func_0021B970(float, float);
extern void func_0021BA80(int, int, int);
extern void block_copy(void *, void *, int);
extern char *D_00275670;

void func_0021B860(void) {
    func_0021B970(1000000.0f, 10000000.0f);
    func_0021BA80(0, 0, 0);
    block_copy(D_00275670 + 0x100, D_00275670 + 0xA0, 0x20);
    block_copy(D_00275670 + 0xC0, D_00275670 + 0xA0, 0x20);
    block_copy(D_00275670 + 0xE0, D_00275670 + 0xA0, 0x20);
}

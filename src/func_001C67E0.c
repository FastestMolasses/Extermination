// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001C8480(int base, short index);
extern short func_00128250(float t);
extern void func_001C8D50(char *p, unsigned char c, float a, float b);
extern void func_001C64F0(char *p, float s);

void func_001C67E0(char *p, int flags, float a, float b) {
    *(short *)(p + 0x2C) = (short)(flags | 0x8000);
    func_001C8480(*(int *)(p + 0x40), *(short *)(p + 0x2C));
    if (a == 0.0f) {
        *(float *)(p + 0x3C) = 1.0f;
        *(short *)(*(char **)(p + 0x110) + 0x8E) = func_00128250(b);
        func_001C8D50(p + 0x110, *(unsigned char *)(p + 0xC), b, 1.0f);
        func_001C64F0(p, 1.0f);
        return;
    }
    *(float *)(p + 0x3C) = a;
    *(short *)(*(char **)(p + 0x110) + 0x8E) = func_00128250(b);
    func_001C8D50(p + 0x110, *(unsigned char *)(p + 0xC), b, a);
}

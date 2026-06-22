// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// 64-bit bitfield packer + tail call. Each of the 7 field inputs is masked to
// 32 bits (dsll32/dsrl32 zero-extension) before being OR'd into a 64-bit word
// at its shift offset, then func_00205A50(arg0, 0x14, packed) is tail-called.
extern void func_00205A50(int, int, unsigned long long);

void func_00205A90(int a0, unsigned int p2, unsigned int p3, unsigned int p4,
                   unsigned int p5, unsigned int p6, unsigned int p7, unsigned int p8) {
    unsigned long long packed =
          (unsigned long long)p2
        | ((unsigned long long)p3 << 2)
        | ((unsigned long long)p4 << 5)
        | ((unsigned long long)p5 << 6)
        | ((unsigned long long)p6 << 9)
        | ((unsigned long long)p7 << 19)
        | ((unsigned long long)p8 << 32);
    func_00205A50(a0, 0x14, packed);
}

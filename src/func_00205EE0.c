// CFLAGS: -O4,p -sdatathreshold 0
typedef unsigned long long u64;
extern void func_00205A50(void *p, int a1, u64 a2);

void func_00205EE0(void *a0, unsigned int a1, unsigned int a2, unsigned int a3) {
    u64 packed = ((u64)a1 << 59) | ((u64)a3 << 48);
    int hdr = 0x51;
    func_00205A50(a0, hdr, ((u64)a2 << 32) | packed);
}

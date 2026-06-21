// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Packs three zero-extended 32-bit args into a 64-bit value and tail-calls
// func_00205A50(p, 0x50, packed). packed = (c<<56)|(b<<48)|(a<<32); each arg is
// truncated to its low 32 bits before shifting (dsll32/dsrl32 zero-extend idiom).
// Eval order c,b,a is load-bearing: it drives the dsll32 24/16/0 sequence to match.
extern void func_00205A50(void *p, long long b, long long packed);

void func_00205EA0(void *p, unsigned int a, unsigned int b, unsigned int c) {
    long long packed =
        ((long long)c << 56) |
        ((long long)b << 48) |
        ((long long)a << 32);
    func_00205A50(p, 0x50, packed);
}

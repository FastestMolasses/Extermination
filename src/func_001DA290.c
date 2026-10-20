// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Copies the 64-byte block D_002531D0 to the stack, runs func_001D1F80(ch, 2,
// 9), then func_001DA1E0(ch, &copy, arg).
// Byte-matched from C with mwcc 2.3.3 (decomp lane round 3, 2026-10-20).

typedef unsigned int u128 __attribute__((mode(TI)));
typedef struct {
    u128 q[4];
} Block64;
extern Block64 D_002531D0;
extern void func_001D1F80(int ch, int a, int b);
extern void func_001DA1E0(int ch, Block64 *blk, int arg);

void func_001DA290(int ch, int arg) {
    Block64 tmp = D_002531D0;
    func_001D1F80(ch, 2, 9);
    func_001DA1E0(ch, &tmp, arg);
}

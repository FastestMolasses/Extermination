// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// One sprite call: func_00207E40(1, 0x8B10, 0x7AF0, 0x20, 0x18,
// 0x80808080, the texture word at page+0x80).
typedef struct { char pad[0x80]; long long q80; } T;
extern void func_00207E40(int a0, int a1, int a2, int a3, int t0, unsigned int t1, long long t2);
void func_00207CD0(T *p) {
    func_00207E40(1, 0x8B10, 0x7AF0, 0x20, 0x18, 0x80808080, p->q80);
}

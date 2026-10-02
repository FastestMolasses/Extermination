// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// One sprite call: func_00207E40(1, 0x8AE0, 0x7B30, 8, 0x10,
// 0x80808080, the texture word at page+0x78).
typedef struct { char pad[0x78]; long long q78; long long q80; } T;
extern void func_00207E40(int a0, int a1, int a2, int a3, int t0, unsigned int t1, long long t2);
void func_00207CA0(T *p) {
    func_00207E40(1, 0x8AE0, 0x7B30, 8, 0x10, 0x80808080, p->q78);
}

// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Guarded non-leaf wrapper: acquire a handle via func_001CA7B0(arg);
// on success (handle >= 0) run a small init sequence, otherwise return.
extern int func_001CA7B0(int);
extern void func_001CA940(int, int);
extern void func_001C7420(int, int, int);
extern void func_001D1F80(int, int, int);
extern void func_001D8C20(int);

void func_001CA990(int *p, int a) {
    int r = func_001CA7B0(a);
    if (r >= 0) {
        func_001D8C20(0);
        func_001C7420((int)p, 0x3F5, 0);
        func_001D1F80(0, 1, 0);
        func_001CA940(r, p[0x11]);
    }
}

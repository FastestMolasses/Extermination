// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001D1F80(int, int, int);
extern void func_001DA1E0(int, int, int);
extern int D_002531D0[];

typedef struct {
    int d[0x10];
} Quad40;

void func_001DA290(int a0, int a1) {
    Quad40 tmp;
    Quad40 *src = (Quad40 *)D_002531D0;
    tmp = *src;
    func_001D1F80(0, 2, 9);
    func_001DA1E0(a0, (int)&tmp, a1);
}

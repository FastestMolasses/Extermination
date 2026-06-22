// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_001029C0(void *);
extern void func_001026D0(void *, int, void *);
extern void func_001C7900(void *, void *, int, int);
extern void func_001CB2C0(int, int, int);
extern void func_001D1F80(int, int, int);
extern void func_001D3F50(int);
extern float D_00250FB0[2];
extern float D_00250FB4[2];
extern float D_00250FB8[2];
extern int *D_00275B40;

typedef struct {
    char head[0x30];
    float x;
    float y;
    float z;
} Frame;

void func_001CB3C0(int arg0) {
    Frame buf;
    char *p;

    p = *(char **)(arg0 + 0x90);
    func_001029C0(&buf);
    buf.x = D_00250FB0[0];
    buf.y = D_00250FB4[0];
    buf.z = D_00250FB8[0];
    func_001026D0(&buf, D_00275B40[*(short *)(arg0 + 0x94)] + 0x90, &buf);
    func_001C7900(&buf, (char *)arg0 + 0x80, 0x3F5, 0);
    func_001CB2C0(arg0, 0x3F3, 0);
    func_001D1F80(0, 1, 0);
    func_001D3F50(*(int *)(p + 0x60));
}

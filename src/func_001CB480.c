// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern int func_001D2910(int);
extern void func_001D8C20(int);
extern void func_001D2830(int, int);
extern void func_001C7420(char *, int, int);
extern void func_001D3BA0(int, int);

void func_001CB480(char *e) {
    int saved;

    saved = func_001D2910(0);
    func_001D8C20(2);
    func_001D2830(0, 0);
    func_001C7420(e, 0x3F5, 1);
    func_001D3BA0(1, *(int *)(e + 0x44));
    func_001D2830(0, saved);
}

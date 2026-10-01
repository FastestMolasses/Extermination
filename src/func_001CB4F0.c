// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern int func_001D2910(int);
extern void func_001D8C20(int);
extern void func_001D2830(int, int);
extern void func_001C7420(char *, int, int);
extern void func_001D1F80(int, int, int);
extern void func_001D38F0(int);

void func_001CB4F0(char *e, int arg) {
    int saved;

    saved = func_001D2910(0);
    func_001D2830(0, 0);
    func_001D8C20(1);
    func_001C7420(e, 0x3F5, 0);
    func_001D1F80(0, 1, 0);
    func_001D38F0(arg);
    func_001D8C20(0);
    func_001D2830(0, saved);
}

// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
extern void func_001D6E60(int, int, int, int);
extern void func_001D1F80(int, int, int);
extern void func_001D1FF0(int, int);
extern void func_001D6F60(int, long long, int);
extern void func_001E10A0(void);
extern void func_001E13E0(int);
extern void func_001D1F20(int);

extern char *D_00275670;

void func_001E1760(int arg0) {
    func_001D6E60(arg0, 0x258000, 8, 8);
    func_001D1F80(arg0, 0, 7);
    func_001D1FF0(arg0, 1);
    func_001D6F60(arg0, *(long long *)(D_00275670 + 0x1E0), 0x80);
    func_001E10A0();
    func_001E13E0(arg0);
    func_001D1F20(arg0);
}

// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001D38F0(int);
extern void func_001D3C30(int);

void func_001CA940(int arg0, int arg1) {
    if (arg0 != 0) {
        if (arg0 & 1) {
            func_001D3C30(arg1);
        } else {
            func_001D38F0(arg1);
        }
    } else {
        func_001D38F0(arg1);
    }
}

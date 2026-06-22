// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
extern int func_001D2E00(int);
extern int func_001D2910(int);
extern int func_001B0070(void);
extern void func_001CB760(void *, int, int);
extern void func_001D2DE0(int, int);

extern char D_007635C0;

void func_001E0D70(void) {
    int temp;

    temp = func_001D2E00(0);
    if (func_001D2910(0x20) != 0) {
        if (!(func_001B0070() & 0x0E000000)) {
            if (temp != 0) {
                func_001CB760(&D_007635C0, 0xFFC000, temp);
                func_001D2DE0(0, 0);
            }
        }
    }
}

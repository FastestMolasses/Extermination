// CFLAGS: -O4,p -sdatathreshold 0
extern void func_001FF0D0(void);
extern char D_0028A798;
extern char D_0028A79E;
extern void func_001AB740(int, void *);

void func_001FF080(int a0, int a1) {
    func_001AB740(2, func_001FF0D0);
    D_0028A798 = a0;
    D_0028A79E = a1;
}
// CFLAGS: -O4,p -sdatathreshold 0
extern char D_008101E0[];
extern char D_008101D0[];
extern char D_00810130[];
extern float D_0081060C;
extern void func_00121A28(char *, int, int);

void func_001AF690(void) {
    char *a0 = D_008101E0;
    int a2 = 0xD0;
    func_00121A28(a0, 0, a2);
    a0 = D_008101D0;
    a2 = 0x10;
    func_00121A28(a0, 0, a2);
    a0 = D_00810130;
    a2 = 0xA0;
    func_00121A28(a0, 0, a2);
    D_0081060C = 1.0f;
}

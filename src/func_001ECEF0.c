// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
extern char D_0081F8F0[0x100];
extern char D_00257090[0x100];
extern char D_00257120[0x100];
extern int *D_00275C30;
extern int *D_00275C34;
extern void func_001CFB50(void *, int, int, float, float, float, float, float);
extern void func_001CFBE0(int, int, void *, void *, int);

void func_001ECEF0(int arg0, int arg1) {
    func_001CFB50(D_0081F8F0, 0, arg0,
                  *(float *)((char *)D_00275C34 + 0x54),
                  *(float *)((char *)D_00275C34 + 0x5C),
                  1.0f, 9.999999974752427e-07f, 3.0f);
    if (*(int *)((char *)D_00275C30 + 0x38) == 0) {
        func_001CFBE0(arg1, 1, D_00257120, D_0081F8F0, 0);
        return;
    }
    func_001CFBE0(arg1, 1, D_00257090, D_0081F8F0, 0);
}

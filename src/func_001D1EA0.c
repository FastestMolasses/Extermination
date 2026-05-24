// CFLAGS: -O4,p -sdatathreshold 4
extern int *D_00275670;
extern int func_001D2910(int);
extern void func_001E0D70(void);
extern void func_001DDA00(void);
extern void func_001CB800(char *, void *, int *, int *);
extern char D_007635C0[8];

void func_001D1EA0(int a0) {
    int *a2;
    int v0;
    if (a0 != 0) {
        v0 = func_001D2910(4);
        if (v0 == 0) {
            func_001E0D70();
            func_001DDA00();
        }
    }
    a2 = D_00275670;
    func_001CB800(D_007635C0, (void *)0, a2, a2 + 1);
}

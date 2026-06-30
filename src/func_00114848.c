// COMPILER: eegcc
// CFLAGS: -O2

extern int D_00241D68;
extern int D_0027B0C0;
extern int D_0027C680;
extern int func_0010EA60(void *);

int func_00114848(int a0, int *a1, int *a2)
{
    int s1 = a0;
    int *s3 = a1;
    int *s4 = a2;
    int ok;

    if (D_00241D68 == 0)
        return -1;

    ok = func_0010EA60(&D_0027B0C0);
    if (s1 == 0 && ok != 0) {
        while (func_0010EA60(&D_0027B0C0) != 0) {
            int n = 0x186A0;
            do {
                n--;
            } while (n != 0);
        }
        ok = 0;
    }

    ok = (unsigned int)ok < 1;
    if (s3 != 0)
        *s3 = D_00241D68;

    if (ok != 0) {
        D_00241D68 = 0;
        if (s4 != 0)
            *s4 = D_0027C680;
    }
    return ok;
}

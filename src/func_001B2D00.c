// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Scans the global float table D_700030F0[] (count at *0x700031E0) for the first
// entry >= arg0[1]; on a hit, checks parallel bitmask tables D_70003170/D_7000316E
// and, when the flag bit is set, writes the matching float (from D_700030F0 or the
// trailing D_700030EC table) into *arg1 and returns 1, else returns 0.
extern void func_0019BC40(int);
extern float D_700030F0;
extern unsigned short D_70003170;
extern unsigned short D_7000316E;
extern float D_700030EC;

int func_001B2D00(float *arg0, float *arg1) {
    int i;
    int j;
    float *p;
    int n;

    func_0019BC40((int)arg0);
    n = *(int *)0x700031E0;
    if (n == 0) {
        return 0;
    }
    p = &D_700030F0;
    i = 0;
    goto cond;
body:
    if (!(p[0] < arg0[1])) {
        if (((&D_70003170)[i] & 1)) {
            *arg1 = (&D_700030F0)[i];
            return 1;
        }
        return 0;
    }
    p++;
    i++;
cond:
    if (i < n) {
        goto body;
    }
    j = i - 1;
    if (((&D_70003170)[j] & 1)) {
        return 0;
    }
    if (j != 0) {
        if (((&D_7000316E)[j] & 1)) {
            *arg1 = (&D_700030EC)[j];
            return 1;
        }
    }
    return 0;
}

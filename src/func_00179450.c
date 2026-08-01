// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Scans the global "event/trigger" tables for an entry that the actor has just
// passed, and reports which kind it is.
//
// First runs func_0019BC40(arg1) (refreshes the table). If the table count at
// *0x700031E0 is nonzero, walks the parallel global tables from index (count-1)
// down to 0: the u16 flag table D_70003170[] and the float table D_700030F0[].
// For each entry whose flag bit 0 is set, if D_700030F0[i] < arg0[0xB4] it
// commits: stores the delta (D_700030F0[i] - arg0[0xB4]) to
// *(float *)(arg0 + 0x258), then returns 2 if the parallel angle/value table
// D_00282250[i] >= 0.62831855 (2*pi/10), else 1. If no entry qualifies,
// returns 0.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906), not the pinned 991202
// (991202 reaches only 75.98%).
extern void func_0019BC40(int);
extern float D_700030F0;
extern unsigned short D_70003170;
extern float D_00282250;

int func_00179450(char *arg0, int arg1) {
    int i;
    unsigned short *flags;
    float *vals;
    float thr;

    func_0019BC40(arg1);
    if (*(int *)0x700031E0 != 0) {
        i = *(int *)0x700031E0 - 1;
        if (i >= 0) {
            flags = &D_70003170 + i;
            vals = &D_700030F0 + i;
            do {
                if (*flags & 1) {
                    thr = *(float *)(arg0 + 0xB4);
                    if (*vals < thr) {
                        *(float *)(arg0 + 0x258) = (&D_700030F0)[i] - thr;
                        if ((&D_00282250)[i] >= 0.62831855f) {
                            return 2;
                        }
                        return 1;
                    }
                }
                i--;
                flags--;
                vals--;
            } while (i >= 0);
        }
    }
    return 0;
}

// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Rumble dispatch by effect id. Indexes a 4-byte-stride effect table
// D_0024D6F0[arg0] -> {big=[0], small=[1], defaultDur=[2]}. If arg1<0,
// stop/clear via func_001B6250(&padblock D_00810E40). If arg1>0, fire
// func_001B61C0(big, small, dur=(short)arg1, force=1) with the caller's
// duration (sign-extended low 16 bits). If arg1==0, fire with the table's
// default duration byte [2]. Far global D_00810E40 forces a HI16/LO16 pair.
// 991202 walls at 67.5% (regalloc: routes the pointer through v1 + an extra
// shift temp in v0); mwcc 2.3.3 keeps the pointer in v0 and matches CW = 100.0.
extern void func_001B61C0(unsigned char big, unsigned char small, long long dur, int force);
extern void func_001B6250(void *padblock);
extern unsigned char D_0024D6F0[];
extern unsigned char D_00810E40[];

void func_001B1E20(int arg0, long long arg1) {
    unsigned char *p;

    p = &D_0024D6F0[arg0 * 4];
    if (arg1 < 0) {
        func_001B6250(D_00810E40);
        return;
    }
    if (arg1 != 0) {
        func_001B61C0(p[0], p[1], (long long)(short)arg1, 1);
        return;
    }
    func_001B61C0(p[0], p[1], p[2], 1);
}

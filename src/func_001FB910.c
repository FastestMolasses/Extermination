// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

extern void DisableDmacHandler(int);
extern int GsPutIMR(int);
extern int SetPgifHandler(void);
extern int iGsPutIMR(void *, int);

extern signed char D_0028215C;
extern int D_002821A0;

int func_001FB910(int arg0, int arg1, int arg2) {
    int buf[4];
    int st = D_0028215C;
    if (st == 1) goto check;
    if (st != 0) return 0;
    D_0028215C = st + 1;
    SetPgifHandler();
    buf[0] = arg0;
    buf[1] = arg1;
    buf[2] = arg2;
    buf[3] = 0;
    DisableDmacHandler(0);
    D_002821A0 = iGsPutIMR(buf, 1);
check:
    if (D_002821A0 == 0) return -1;
    if (GsPutIMR(D_002821A0) >= 0) return 1;
    DisableDmacHandler(0);
    return 0;
}

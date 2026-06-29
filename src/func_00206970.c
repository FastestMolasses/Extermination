// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// GS IMR set/poll: validate count, build param buffer (arg1, arg0, n, 0),
// disable DMAC handler, submit via iGsPutIMR, busy-poll GsPutIMR until it
// returns negative, restore DMAC handler, return the count.

extern void DisableDmacHandler(int);
extern int GsPutIMR(int);
extern int iGsPutIMR(void *, int);

int func_00206970(int arg0, int arg1, int n) {
    int buf[4];
    int id;
    if (n <= 0) return 0;
    buf[0] = arg1;
    buf[1] = arg0;
    buf[2] = n;
    buf[3] = 0;
    DisableDmacHandler(0);
    id = iGsPutIMR(buf, 1);
    while (GsPutIMR(id) >= 0)
        ;
    DisableDmacHandler(0);
    return n;
}

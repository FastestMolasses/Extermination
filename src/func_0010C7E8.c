// COMPILER: eegcc
// CFLAGS: -O2
// Build a 4-word DMA request on the stack and submit via sceSifDmaStat(1, &req).
extern int D_00276CA0;
extern int sceSifDmaStat(int chan, void *req);

int func_0010C7E8(unsigned short a0, int a1, int a2) {
    int req[4];
    req[1] = a1;
    req[0] = a0;
    req[2] = a2;
    req[3] = (int)&D_00276CA0 | 0x20000000;
    return sceSifDmaStat(1, req);
}

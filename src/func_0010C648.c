// COMPILER: eegcc
// CFLAGS: -O2
extern int D_00241C68;
extern int D_00276C98;
extern int D_00276890;
extern int D_0027D370;
extern int D_00276C90;
extern void func_0010C5C8(int *a0);
extern int EndOfHeap(int *a0);
extern int _iSetAlarm(int *a0);
extern int CreateThread(int a0, int *a1);
extern int ReleaseWaitThread(void);
extern void DisableDispatchThread(int a0, int a1);
extern void RFU063(int a0);

int func_0010C648(void) {
    int al[12];
    int heap[8];
    int r;
    if (D_00241C68 > 0) {
        return -1;
    }
    heap[0] = 0;
    heap[1] = 0xFF;
    r = EndOfHeap(heap);
    D_00276C98 = r;
    if (r < 0) {
        return -1;
    }
    al[1] = (int)func_0010C5C8;
    al[2] = (int)&D_00276890;
    al[3] = 0x400;
    al[4] = (int)&D_0027D370;
    al[5] = 0;
    r = _iSetAlarm(al);
    D_00241C68 = r;
    if (r < 0) {
        RFU063(D_00276C98);
        return -1;
    }
    CreateThread(r, &D_00276C90);
    DisableDispatchThread(ReleaseWaitThread(), 1);
    return D_00241C68;
}

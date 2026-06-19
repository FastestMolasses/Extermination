// COMPILER: eegcc
// CFLAGS: -O2
extern int RFU116(int a0);
extern void SetVSyncFlag(int a0, int a1);
extern void func_0010CE28(void);

int func_001104C0(void) {
    if (RFU116(4) & 0x40000) {
        SetVSyncFlag(4, 0x40000);
        func_0010CE28();
        return 1;
    }
    return 0;
}

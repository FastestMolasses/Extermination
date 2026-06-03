// Disarm an active trigger record (counterpart of func_001B61C0). If the record
// is live (+0x12 set) and currently armed (+0x16 set), clear its flags/selectors
// and notify func_00111018 with the stored channel pair (+0x4, +0x8).
extern void func_00111018(int chan0, int chan1, void *rec);

void func_001B6250(unsigned char *rec) {
    if (rec[0x12] == 0) return;
    if (rec[0x16] == 0) return;
    rec[0x16] = 0;
    *(short *)(rec + 0x28) = 0;
    rec[0x18] = 0;
    rec[0x19] = 0;
    func_00111018(*(int *)(rec + 4), *(int *)(rec + 8), rec + 0x18);
}

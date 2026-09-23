extern int *D_00275670;

void func_001D2160(char *a0) {
    // Original: reads the pointer D_00275670, loads its word at +8, stores 0x20 to a0[3]
    // One register is both the base AND the destination of that +8 load
    int *a1 = D_00275670;
    int val = *(int *)((char *)a1 + 8);  // forces that base/destination reuse
    a0[3] = 0x20;                          // byte store of 0x20
    *(int *)(a0 + 4) = val;               // word store of val
    *(short *)a0 = 0;                      // halfword store of zero
}

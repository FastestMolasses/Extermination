extern int *D_00275670;

void func_001D2160(char *a0) {
    // Expected: loads D_00275670 into a1, then loads a1[8] into a1, stores 0x20 to a0[3]
    // The lw a1, 8(a1) uses a1 for both the base AND destination
    int *a1 = D_00275670;
    int val = *(int *)((char *)a1 + 8);  // forces the lw a1->val pattern
    a0[3] = 0x20;                          // sb v1 = 0x20
    *(int *)(a0 + 4) = val;               // sw val
    *(short *)a0 = 0;                      // sh $zero
}

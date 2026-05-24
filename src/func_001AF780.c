extern short D_00275BCC;
extern int *D_00275BD0;

int func_001AF780(void) {
    short v0;
    int *v1;
    v0 = D_00275BCC;
    if (v0 < 0x1F) goto exit_zero;
    v1 = D_00275BD0;
    D_00275BCC = v0 - 1;
    D_00275BD0 = (int *)((char *)v1 + 4);
    return *(int *)v1;
exit_zero:
    return 0;
}

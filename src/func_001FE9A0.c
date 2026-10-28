extern int D_00275C64;
extern int func_00114848(int a0, int *a1, int *a2);

/* Request poll wrapper: runs func_00114848(mode, &cmd, &result) (-1 = no
 * request pending, 0 = still busy, 1 = finished) and, when it reports 1,
 * latches the result word into D_00275C64. Returns func_00114848's status
 * unchanged (callers test it against 0). */
int func_001FE9A0(int a0, int *a1, int *a2) {
    int *s0 = a2;
    int v0 = func_00114848(a0, a1, a2);
    if (v0 == 1) {
        D_00275C64 = *s0;
    }
    return v0;
}

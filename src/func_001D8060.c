// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Linear table lookup: D_00275670+0x220 is the entry array (0x80-byte entries,
// up to 0x20). Match by id at offset 0xC; -1 sentinel returns null. The pointer
// base is computed BEFORE the -1 check (CW hoists lw gp_rel above the branch,
// filling the bne delay slot with the +0x220 addiu).
extern char *D_00275670;

char *func_001D8060(int a0) {
    char *p;
    int i;

    p = D_00275670 + 0x220;
    if (a0 == -1) {
        return 0;
    }
    for (i = 0; i < 0x20; i++) {
        if (*(int *)(p + 0xC) == a0) {
            return p;
        }
        p += 0x80;
    }
    return 0;
}

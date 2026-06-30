// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Predicate over an entity arg0. Returns 0 ("not active") when state byte
// arg0[4]==5 && arg0[5]==1. Otherwise returns 1 if ANY of: the float at
// arg0+0x220 is <= 0.0; flag arg0[0x25F] set; func_0021BB00(arg0) nonzero;
// global flag D_008106F1 set; or the mode byte arg0[0x1F0] is 0x3C or 0x3D.
// Returns 0 if none hold.
//
// st is declared int (loaded via lbu) so mwcc 2.3.3 re-masks the byte (andi)
// between the 0x3C and 0x3D comparisons, matching the target.
extern int func_0021BB00(char *p);
extern unsigned char D_008106F1;

int func_00182B30(char *arg0) {
    int st;

    if (*(unsigned char *)(arg0 + 4) == 5 && *(unsigned char *)(arg0 + 5) == 1) {
        return 0;
    }
    if (*(float *)(arg0 + 0x220) <= 0.0f ||
        *(unsigned char *)(arg0 + 0x25F) != 0 ||
        func_0021BB00(arg0) != 0 ||
        D_008106F1 != 0 ||
        (st = *(unsigned char *)(arg0 + 0x1F0)) == 0x3C ||
        st == 0x3D) {
        return 1;
    }
    return 0;
}

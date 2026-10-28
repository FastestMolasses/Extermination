// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 69.87% NEARMISS).
// Fixes read from the original instructions:
// - The bail-outs are grouped OR conditions, not separate ifs: (health <= 0
//   || type byte == 0x63) and (D_008106F1 || func_0021BB00 || kind 0x3C/0x3D/
//   0xB..0xD) each share one `return 1` exit.
// - The first block tests D_0081083C == 0 first (clear D_008106BC) with the
//   `return 1` in the else arm.
// - The kind byte is held in an int (no re-mask between the compares).
// - The final test is a plain float truth test (`f || g`), which gives the
//   original's compare operand order.
//
// Gate/eligibility check on an entity (arg0). Returns 1 (blocked) when: the
// pending flag D_008106BC is set and D_0081083C is still set (otherwise the
// pending flag is cleared); float[arg0+0x220] <= 0; byte[arg0+0xF] == 0x63;
// D_0081083C is set (then D_008106BC is set to 1); D_008106F1 is set;
// func_0021BB00(arg0) returns nonzero; or byte[arg0+0x1F0] is 0x3C, 0x3D or
// in [0xB, 0xE). Otherwise, if either float at arg0+0x22C or arg0+0x224 is
// nonzero, zeroes both and sets byte[arg0] = 1; returns 0.
extern int func_0021BB00(char *p);
extern unsigned char D_008106BC;
extern unsigned char D_008106F1;
extern unsigned char D_0081083C;

int func_00182BF0(char *arg0) {
    int v;

    if (D_008106BC != 0) {
        if (D_0081083C == 0) {
            D_008106BC = 0;
        } else {
            return 1;
        }
    }
    if (*(float *)(arg0 + 0x220) <= 0.0f || *(unsigned char *)(arg0 + 0xF) == 0x63) {
        return 1;
    }
    if (D_0081083C != 0) {
        D_008106BC = 1;
        return 1;
    }
    if (D_008106F1 != 0 || func_0021BB00(arg0) != 0 ||
        (v = *(unsigned char *)(arg0 + 0x1F0)) == 0x3C || v == 0x3D ||
        (unsigned int)(v - 0xB) < 3) {
        return 1;
    }
    if (*(float *)(arg0 + 0x22C) || *(float *)(arg0 + 0x224)) {
        *(float *)(arg0 + 0x22C) = 0.0f;
        *(float *)(arg0 + 0x224) = 0.0f;
        *(char *)(arg0 + 0) = 1;
    }
    return 0;
}

// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Does this entity's kind (+0x1F0) block the player's action? Returns 1 for
// kinds 8, 9, 0x0A, 0x14, 0x2C (unless its +0x0D byte is 2), 0x2E,
// 0x11..0x13, 0x15..0x16, 0x18..0x19, 0x1B..0x1C, 0x1E..0x20, 0x28 and
// 0x23..0x26. Kind 0x0F returns 1 unless it is in state (1, 8) with
// sub-state 2, 4 or 5, which return 0. Every other kind returns 0.
int func_0021BB00(unsigned char *e) {
    unsigned char k = e[0x1F0];

    if (k == 8 || k == 9 || k == 10 || k == 0x14 || (k == 0x2C && e[0xD] != 2)
        || k == 0x2E || k == 0x11 || k == 0x12 || k == 0x13 || k == 0x15 || k == 0x16
        || k == 0x18 || k == 0x19 || k == 0x1B || k == 0x1C
        || k == 0x1E || k == 0x1F || k == 0x20 || k == 0x28
        || k == 0x23 || k == 0x24 || k == 0x25 || k == 0x26) {
        return 1;
    }
    if (k == 0x0F) {
        if (e[4] == 1 && e[5] == 8 && (e[6] == 2 || e[6] == 4 || e[6] == 5)) {
            return 0;
        }
        return 1;
    }
    return 0;
}

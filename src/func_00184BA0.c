// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8
// SPAD: 0x70003B98

//
// Structure notes (earlier lane): the loop counter is an int loaded once from
// the short D_00275B64, and the 0x70003B8D writes are bytes.
// MATCH NOTE (m2-matching lane, 98.23% -> 100.0%): (1) declaring obj, winner,
// pp, r, cnt in that order gives the target's s0/s1/s2/s3 coloring (idiom-3);
// (2) 0x70003B8D (global _SPAD_SYMS) and 0x70003B98 (`// SPAD: 0x70003B98`)
// are relocated externs, over-declared past the 8-byte sdata threshold, so the
// scratchpad-base lui (upper half 0x7000) of the score reload is no longer
// speculated into the result-2 test slot. The score is cleared as `= 0.0f` (an
// integer zero store).
//
// Picks the best target object: bails if any of three gate flags is set
// (0x70003B8D, D_0028A9A0, D_008106EF). Walks the active-object list (D_00275B5C,
// count D_00275B64); for each live/visible/unflagged object calls func_00183EF0
// (the per-object test). Result 2 = immediate hit (flag obj+0xB=4, set 0x70003B8D=3,
// return 1); otherwise keep the object with the smallest score parked at 0x70003B98.
// After the scan, commits the best (winner) the same way, else returns 0.
extern int func_00183EF0(int, unsigned char *);
extern unsigned char **D_00275B5C;
extern short D_00275B64;
extern short D_0028A9A0[256];
extern unsigned char D_008106EF[256];
extern unsigned char D_70003B8D[16];
extern float D_70003B98[4];

int func_00184BA0(int arg0) {
    float best;
    float v;
    unsigned char *obj;
    unsigned char *winner;
    unsigned char **pp;
    int r;
    int cnt;

    best = 10000.0f;
    winner = 0;
    if (D_70003B8D[0] != 0) {
        return 0;
    }
    if (D_0028A9A0[0] != 0) {
        return 0;
    }
    if (D_008106EF[0] != 0) {
        return 0;
    }
    cnt = D_00275B64;
    pp = D_00275B5C;
    D_70003B98[0] = 0.0f;
    if (cnt != 0) {
        do {
            obj = *pp;
            cnt -= 1;
            pp += 1;
            if ((obj[0] & 1) && (obj[2] & 0x80) && (obj[0xB] == 0)) {
                r = func_00183EF0(arg0, obj);
                if (r != 0) {
                    if (r == 2) {
                        obj[0xB] = 4;
                        D_70003B8D[0] = 3;
                        return 1;
                    }
                    v = D_70003B98[0];
                    if (v < best) {
                        best = v;
                        winner = obj;
                    }
                }
            }
        } while (cnt != 0);
    }
    if (winner != 0) {
        winner[0xB] = 4;
        D_70003B8D[0] = 3;
        return 1;
    }
    return 0;
}

// NEARMISS func_00184BA0  (vram 0x00184BA0, 0x174 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 98.23% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-allocation permutation (callee-saved coloring cnt/pp/obj/winner across s0-s3); body byte-identical after int-counter + byte-store fixes, not the clean-store nop -> permuter, not 2.3.3
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS 98.2% (mwcc 2.3.3; pinned 991202 = 88.8%). Body is byte-identical to
// the target after two structural fixes: (1) the loop counter is an int (loaded
// once via lh from the short D_00275B64) -- declaring it short made mwcc re-emit
// the dsll32/dsra32 sign-extend dance each iteration; (2) the writes to
// 0x70003B8D are byte (sb), not word -- *(int*) made mwcc emit swr/swl. gp-rel
// vs absolute is matched by keeping D_00275B5C/D_00275B64 small (gp-rel) and
// declaring D_0028A9A0/D_008106EF as arrays (absolute). Sole residual is a
// register-allocation permutation: target {cnt->s3, pp->s2, obj->s0, winner->s1},
// mwcc {cnt->s0, pp->s1, obj->s2, winner->s3}. Same instruction stream -> permuter.
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

int func_00184BA0(int arg0) {
    float best;
    float v;
    int cnt;
    int r;
    unsigned char **pp;
    unsigned char *obj;
    unsigned char *winner;

    best = 10000.0f;
    winner = 0;
    if (*(unsigned char *)0x70003B8D != 0) {
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
    *(int *)0x70003B98 = 0;
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
                        *(char *)0x70003B8D = 3;
                        return 1;
                    }
                    v = *(float *)0x70003B98;
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
        *(char *)0x70003B8D = 3;
        return 1;
    }
    return 0;
}

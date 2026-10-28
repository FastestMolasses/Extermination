// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// The ground height is read into the first float of a 4-float local (the
// original's frame slot for it is at the bottom of a 16-byte local area).
//
// Ground probe below an actor (steering / movement fallback used by
// func_0012F100, func_0012F6C0, func_001305B0, func_0013DD40, func_0013E6D0).
// func_001B2B80(p, v, &ground) probes the collision at v and returns flag
// bits: bit 0 = a ground height was found (written to ground), bit 2 = the
// probe hit an object, published at scratchpad pointer 0x700031D0.
//   no ground found                                   -> 2
//   object hit whose short at +0x1A has bit 0x2000    -> 1
//   p.y (+0xB4) - r is at or above the ground height  -> 2
//   p.y - r below the ground height                   -> 0

typedef struct Obj {
    unsigned char pad0[0xB4];
    float y;                        /* +0xB4 */
} Obj;

extern int func_001B2B80(Obj *p, void *v, float *ground);

int func_001B3250(Obj *p, void *v, float r) {
    float ground[4];
    int hit;

    hit = func_001B2B80(p, v, ground);
    if (!(hit & 1)) {
        return 2;
    }
    if ((hit & 4) && (*(short *)(*(char **)0x700031D0 + 0x1A) & 0x2000)) {
        return 1;
    }
    if (!(p->y - r < ground[0])) {
        return 2;
    } else {
        return 0;
    }
}

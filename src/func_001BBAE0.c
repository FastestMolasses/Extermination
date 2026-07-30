// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// SEMANTICS:
// One-shot initializer + ready-poll for a global 4-word request block at 0x002821B0.
// `st[4]` is the caller's latch byte: 0 = not yet issued, 1 = issued.
//   st[4] == 1 -> skip straight to the poll.
//   st[4] == 0 -> issue the request, then fall through to the poll.
//   anything else -> return 0.
// Issue path: D_002821B0 = 2 (request kind), D_002821B4 = 1 (status := busy), then derive a
// selector from the object's byte at +0x56, masked to its low 6 bits and kept as a SIGNED char:
//   kind 0 -> 0x80000006   kind 1 -> 0x80000000   kind 2 -> 0x80000002
//   kind 3 -> 0x80000008   kind 4 -> 0x8000000A   kind 5 -> 0x80000004
// stored to D_002821B8 (the 0x80000000 bit looks like a "negative / signed magic" tag), then
// D_002821BC = 0 (result slot cleared) and st[4] = 1 (latch).
// kind >= 6 returns 1 WITHOUT issuing and WITHOUT latching st[4] — the poll is skipped entirely.
// Poll: returns 1 once D_002821B4 has been advanced to 2 by whatever services the request,
// 0 while it is still 1.
//
// DEAD BRANCH, faithfully reproduced: the `kind == 0xFF` test can never fire, because `kind` is
// the result of `& 0x3F` and so is always 0..63. CW emitted it anyway (`addiu v1,zero,0xff` +
// `bne`), so it is kept here — this is original-source dead code, not a decode error.

extern volatile int D_002821B0;
extern volatile int D_002821B4;
extern int D_002821B8;
extern int D_002821BC;

int func_001BBAE0(char *obj, unsigned char *st)
{
    int val;
    char kind;

    switch (st[4]) {
    case 0:
        D_002821B0 = 2;
        D_002821B4 = 1;
        kind = *(char *)(obj + 0x56) & 0x3F;
        if (kind == 0xFF) {
            return 1;
        }
        switch (kind) {
        case 0:
            val = 0x80000006;
            break;
        case 1:
            val = 0x80000000;
            break;
        case 2:
            val = 0x80000002;
            break;
        case 3:
            val = 0x80000008;
            break;
        case 4:
            val = 0x8000000A;
            break;
        case 5:
            val = 0x80000004;
            break;
        default:
            return 1;
        }
        D_002821B8 = val;
        D_002821BC = 0;
        st[4] = 1;
        /* fall through */
    case 1:
        if (D_002821B4 == 2) {
            return 1;
        }
        /* fall through */
    default:
        return 0;
    }
}

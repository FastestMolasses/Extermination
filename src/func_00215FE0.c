// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Records up to five "hit" entries into a per-object event buffer, one per
// active global flag (D_00810C82..D_00810C86). Counter byte at p[0x18] is
// reset to 0, then for each set flag the pre-increment counter is used as
// the slot index into the byte array at p[0x50], storing the flag's ordinal
// (0..4). The explicit `int v` temp for the stored ordinal is the matching
// idiom: it pins the constant to $a2 and the loaded counter to $a1, matching
// the target register coloring (without it mwcc swaps a1<->a2).

extern unsigned char D_00810C82;
extern unsigned char D_00810C83;
extern unsigned char D_00810C84;
extern unsigned char D_00810C85;
extern unsigned char D_00810C86;

void func_00215FE0(unsigned char *p)
{
    unsigned char n;
    int v;

    p[0x18] = 0;
    if (D_00810C82) {
        v = 0;
        n = p[0x18];
        p[0x18] = n + 1;
        p[n + 0x50] = v;
    }
    if (D_00810C83) {
        v = 1;
        n = p[0x18];
        p[0x18] = n + 1;
        p[n + 0x50] = v;
    }
    if (D_00810C84) {
        v = 2;
        n = p[0x18];
        p[0x18] = n + 1;
        p[n + 0x50] = v;
    }
    if (D_00810C85) {
        v = 3;
        n = p[0x18];
        p[0x18] = n + 1;
        p[n + 0x50] = v;
    }
    if (D_00810C86) {
        v = 4;
        n = p[0x18];
        p[0x18] = n + 1;
        p[n + 0x50] = v;
    }
}

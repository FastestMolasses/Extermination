// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Maps a key byte arg0[0xD] to one of 8 table indices via a comparison-chain
// switch (returns 1 on no match), then copies a 16-byte entry from
// D_002535F0[idx] into the actor struct at arg0: three floats (entry+4/8/C ->
// +0xA0/A4/A8), a const 0x3F800000 -> +0xAC, and the entry's int word -> +0x28.
//
// mwcc 2.3.3 byte-matches (objdiff 100.0%); the pinned 991202 caps at 80.5%
// (it reschedules the table base/index address arithmetic). KEY idioms:
// (1) case labels written in ASCENDING order within each group -- mwcc emits the
//     comparison chain reversed, matching CW's descending test order.
// (2) the table must be a named 16-byte struct array indexed by idx
//     (&D_002535F0[idx]); this makes mwcc compute sll/lui+addiu/addu in the same
//     order/registers as CW. A char* + idx*0x10 expression mis-schedules.
typedef struct { int w; float x, y, z; } Entry;
extern Entry D_002535F0[];

int func_001E23A0(unsigned char *arg0) {
    int idx;
    Entry *e;

    switch (arg0[0xD]) {
    case 0x3B:
    case 0x3D:
    case 0x3E:
    case 0x3F:
    case 0x40:
        idx = 0;
        break;
    case 0x47:
    case 0x48:
    case 0x49:
        idx = 1;
        break;
    case 0x4E:
    case 0x4F:
    case 0x50:
    case 0x51:
        idx = 2;
        break;
    case 0x54:
    case 0x55:
        idx = 3;
        break;
    case 0x58:
    case 0x59:
    case 0x5A:
        idx = 4;
        break;
    case 0x61:
        idx = 5;
        break;
    case 0x68:
        idx = 6;
        break;
    case 0x6A:
        idx = 7;
        break;
    default:
        idx = -1;
        break;
    }
    if (idx == -1) {
        return 1;
    }
    e = &D_002535F0[idx];
    *(float *)(arg0 + 0xA0) = e->x;
    *(float *)(arg0 + 0xA4) = e->y;
    *(float *)(arg0 + 0xA8) = e->z;
    *(int *)(arg0 + 0xAC) = 0x3F800000;
    *(int *)(arg0 + 0x28) = e->w;
    return 0;
}

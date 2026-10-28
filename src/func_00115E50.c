// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 63.70% NEARMISS).
// Fixes read from the original instructions:
//  - Typed records as in the byte-matched siblings: the 0x6A-byte Voice table
//    D_0027CCC0[48] indexed directly, the D_00281AC0 sequencer globals as a struct
//    of byte pointers (f00 header, f04 tone record, f08, f0C track, f18 table), the
//    D_0027F740 Tempo struct with its 64-bit masks and f34 counter, and an Event
//    record, instead of a local pointer-array copy and byte offsets.
//  - func_001177E8 takes the event (its definition and the original keep ev in a0),
//    and func_00117BA0 is called as func_00117BA0(0, 0) and declared returning int
//    (the original shifts its result arithmetically).
//  - The bail-out and the normal end each advance ev->f08 by 3 and return directly
//    (-1 / 0); the old shared goto tail laid out the blocks differently.
//  - The non-0xFF arms share one trailing `ev->f2A = 0;`, the 0xFF arm is the chained
//    `ev->f2A = ev->f2C = ...`, and the fifth func_00117918 argument is chosen into
//    its own local before the call.
//
// Sequencer note-on handler for one event. Bails out (-1) when the track is off
// (f0C[3] == 0), the event's tone is flagged (D_0027C6C0[ev->f24].t08 & 1) or the
// event's note is below the header base note. Header byte 0 picks the voice range:
// 0xFF = the single slot (note - base), bit 0x80 = slots 0..(b0 - 0x80) all keyed,
// otherwise slots 0..b0 with only the first allocated slot keyed. For each slot that
// func_001178C0 accepts, allocates a voice (func_001172B8), fills its whole 0x6A-byte
// record from the event / header / tone / track bytes, issues func_001157F0
// commands 6 (func_00117918's result scaled by 44100/48000), 1, 5 and 3, and
// updates the D_0027F740 mask words and counter. Advances ev->f08 by 3.

typedef struct {
    unsigned short f00, f02, f04, f06, f08, f0A, f0C, f0E;
    unsigned short f10, f12, f14, f16, f18, f1A, f1C, f1E;
    unsigned short f20, f22, f24, f26, f28, f2A, f2C, f2E;
    unsigned short f30, f32, f34, f36, f38, f3A, f3C, f3E;
    unsigned short f40, f42, f44, f46, f48, f4A, f4C, f4E;
    unsigned short f50, f52, f54, f56, f58, f5A, f5C, f5E;
    unsigned short f60, f62, f64, f66, f68;
} Voice;

typedef struct {
    unsigned char *f00;
    unsigned char *f04;
    unsigned char *f08;
    unsigned char *f0C;
    int f10;
    int f14;
    unsigned char *f18;
} SeqGlobals;

typedef struct {
    long f00;
    long f08;
    long f10;
    long f18;
    long f20;
    long f28;
    int f30;
    int f34;
    char pad_38[2];
    unsigned short f3A;
} Tempo;

typedef struct { int t00; int t04; int t08; } Tone;

typedef struct {
    unsigned char f00;
    unsigned char f01;
    unsigned char f02;
    unsigned char f03;
    unsigned char f04;
    char pad_05[3];
    int f08;
    char pad_0C[0x0C];
    unsigned short f18;
    char pad_1A[0x0A];
    unsigned short f24;
    char pad_26[4];
    unsigned short f2A;
    unsigned short f2C;
} Event;

extern SeqGlobals D_00281AC0;
extern Voice D_0027CCC0[48];
extern Tempo D_0027F740;
extern Tone D_0027C6C0[];
extern unsigned short D_00242630[];

extern int func_001177E8(Event *ev);
extern int func_001178C0(int a0, int idx, int key);
extern int func_001172B8(int a0);
extern int func_00117BA0(int a0, int a1);
extern int func_00117918(int a0, int a1, int a2, int a3, int a4);
extern int func_001157F0(int a0, int a1, int a2, int a3);
extern int func_001179E0(int idx, Event *ev);

int func_00115E50(Event *ev) {
    int i;
    int inv = 0;
    int key;
    int t;
    long mask;

    if (ev->f03 == 0) {
        func_001177E8(ev);
        return -1;
    }

    if (D_00281AC0.f0C[3] == 0 ||
        (D_0027C6C0[ev->f24].t08 & 1) != 0 ||
        D_00281AC0.f00[6] - ev->f02 > 0) {
        ev->f08 += 3;
        return -1;
    }

    if (D_00281AC0.f00[0] == 0xFF) {
        ev->f2A = ev->f2C = ev->f02 - D_00281AC0.f00[6];
    } else {
        if (D_00281AC0.f00[0] & 0x80) {
            inv = 1;
            ev->f2C = D_00281AC0.f00[0] - 0x80;
        } else {
            ev->f2C = D_00281AC0.f00[0];
        }
        ev->f2A = 0;
    }

    for (i = ev->f2A; i < ev->f2C + 1; i++) {
        if (func_001178C0(D_00281AC0.f00[0], i, ev->f02) != 1) {
            continue;
        }
        key = func_001172B8(i);
        if (key == -1) {
            break;
        }

        mask = (long)1 << key;
        D_00281AC0.f04 += i * 16;
        if (D_00281AC0.f04[0xF] & 1) {
            D_0027CCC0[key].f08 = 0;
            D_0027CCC0[key].f0C = 1;
        } else {
            D_0027CCC0[key].f08 = 0;
            D_0027CCC0[key].f0C = 0;
        }

        D_0027CCC0[key].f00 = 1;
        D_0027CCC0[key].f02 = ev->f02;
        D_0027CCC0[key].f04 = ev->f00 & 0xF;
        D_0027CCC0[key].f06 = ev->f18;
        D_0027CCC0[key].f0A = D_0027F740.f34;
        D_0027CCC0[key].f0E = i;
        D_0027CCC0[key].f12 = 0;
        D_0027CCC0[key].f1A = 1;
        D_0027CCC0[key].f1C = 0;
        D_0027CCC0[key].f1E = D_00281AC0.f04[1];
        D_0027CCC0[key].f20 = D_00281AC0.f04[0];
        D_0027CCC0[key].f22 = ev->f24;
        D_0027CCC0[key].f28 = D_00281AC0.f0C[0xE];
        D_0027CCC0[key].f2A = D_00281AC0.f00[1];
        D_0027CCC0[key].f2C = D_00281AC0.f18[ev->f03 + 2];
        D_0027CCC0[key].f2E = D_00281AC0.f04[0xB];
        D_0027CCC0[key].f30 = D_00281AC0.f08[0];
        D_0027CCC0[key].f32 = D_00242630[func_00117BA0(0, 0) >> 2];
        D_0027CCC0[key].f34 = D_00281AC0.f0C[3];
        D_0027CCC0[key].f36 = (signed char)D_00281AC0.f04[3];
        D_0027CCC0[key].f38 = D_00281AC0.f0C[0xA];
        D_0027CCC0[key].f3A = D_00281AC0.f04[0xD];
        D_0027CCC0[key].f3C = D_00281AC0.f0C[0xC];
        D_0027CCC0[key].f3E = ev->f04;
        D_0027CCC0[key].f40 = D_00281AC0.f04[2];
        D_0027CCC0[key].f42 = 0;
        D_0027CCC0[key].f44 = 0;
        D_0027CCC0[key].f4A = D_00281AC0.f04[0xA];
        D_0027CCC0[key].f4C = D_00281AC0.f0C[4];
        D_0027CCC0[key].f4E = 0x78;

        if (D_00281AC0.f00[0] == 0xFF) {
            D_0027CCC0[key].f66 = 1;
            D_0027CCC0[key].f68 = 0x40;
        } else {
            D_0027CCC0[key].f66 = 0;
            D_0027CCC0[key].f68 = D_00281AC0.f00[7];
        }

        if (D_00281AC0.f0C[0xB] == 0x7F) {
            D_0027CCC0[key].f18 = 1;
        }

        if ((D_00281AC0.f04[0xF] & 0x20) && D_00281AC0.f0C[9] != 0) {
            if (D_00281AC0.f04[0xF] & 0x40) {
                D_0027CCC0[key].f10 = D_00281AC0.f00[5];
            } else {
                D_0027CCC0[key].f10 = D_00281AC0.f04[0xE];
            }
            D_0027CCC0[key].f14 = 1;
            D_0027CCC0[key].f16 = D_00281AC0.f0C[9];
        } else {
            D_0027CCC0[key].f14 = 0;
            D_0027CCC0[key].f16 = 0;
        }

        {
            int pan;
            if (D_00281AC0.f04[0xF] & 0x10) {
                pan = D_00281AC0.f00[4];
            } else {
                pan = D_00281AC0.f04[0xD];
            }
            t = func_00117918(D_00281AC0.f04[2], ev->f02, (signed char)D_00281AC0.f04[3],
                              D_00281AC0.f0C[0xA], pan);
        }
        func_001157F0(6, key, t * 44100 / 48000, 0);

        t = func_001179E0(key, ev);
        func_001157F0(1, key, t >> 16, t & 0xFFFF);

        func_001157F0(5, key, (*(unsigned short *)(D_00281AC0.f04 + 4) + D_0027C6C0[ev->f24].t08) << 3, 0);

        func_001157F0(3, key, *(unsigned short *)(D_00281AC0.f04 + 6),
                      *(unsigned short *)(D_00281AC0.f04 + 8));

        D_0027F740.f20 |= mask;
        if (D_00281AC0.f04[0xF] & 0x80) {
            D_0027F740.f00 |= mask;
        } else {
            D_0027F740.f00 &= ~mask;
        }
        D_0027F740.f10 &= ~mask;
        D_0027F740.f34++;
        D_00281AC0.f04 -= i * 16;

        if (inv == 0) {
            break;
        }
    }

    ev->f08 += 3;
    return 0;
}

// COMPILER: eegcc
// CFLAGS: -O2

// Byte-matched (objdiff 100%), matching wave 2026-10-27 (was a 68.60% NEARMISS).
// Fixes read from the original instructions:
//  - Rewritten in the house spelling of the voice/channel family: typed
//    Voice D_0027CCC0[48], Event argument, Song D_00281AC0 (pointer fields read
//    directly, so the original keeps the record's address in a saved register)
//    and Tempo D_0027F740 (64-bit mask words f00 / f10 / f20, counter f34)
//    instead of a local `unsigned char **` alias and byte-offset casts. The
//    "register-allocation wall" was this spelling, not the compiler.
//  - func_00117BA0 takes two arguments; the original passes (4, 0) and shifts
//    the int result arithmetically.
//  - func_001176E0 receives the event (its definition takes the Seq record).
//  - The fifth argument of func_00117918 is selected into its own local
//    (reusing the result temporary moved it through an extra register).
//
// Sequencer note-on event. When ev->f03 is 0 the event is a note-off handled
// by func_001176E0. Otherwise the program slot ev->f02 - song header byte 6 is
// selected (negative: skip), the program record pointer D_00281AC0.f04 is
// advanced to it, a channel is allocated with func_00117428, and the voice
// record is filled from the event, the program record, the header and
// D_00281AC0.f0C / f08 / f18. Pitch (func_00117918, scaled 44100/48000), the
// packed volume pair (func_001179E0), the tone address and the ADSR pair are
// pushed through func_001157F0, and the voice's bit is set in D_0027F740.f20
// and set or cleared in f00 / f10 per the program flags. Restores the program
// pointer, bumps D_0027F740.f34, advances ev->f08 by 4 and returns 0.

typedef struct {
    unsigned short f00, f02, f04, f06, f08, f0A, f0C, f0E;
    unsigned short f10, f12, f14, f16, f18, f1A, f1C, f1E;
    unsigned short f20, f22, f24, f26, f28, f2A, f2C, f2E;
    unsigned short f30, f32, f34, f36, f38, f3A, f3C, f3E;
    unsigned short f40, f42, f44, f46, f48, f4A, f4C, f4E;
    unsigned short f50, f52, f54, f56, f58, f5A, f5C, f5E;
    unsigned short f60, f62, f64, f66, f68;
} Voice;                        /* 0x6A */

typedef struct {
    unsigned char *f00;         /* 0x00 */
    unsigned char *f04;         /* 0x04 */
    unsigned char *f08;         /* 0x08 */
    unsigned char *f0C;         /* 0x0C */
    char pad_10[0x08];
    unsigned char *f18;         /* 0x18 */
} Song;

typedef struct {
    unsigned long f00;          /* 0x00 */
    char pad_08[0x08];
    unsigned long f10;          /* 0x10 */
    char pad_18[0x08];
    unsigned long f20;          /* 0x20 */
    char pad_28[0x0C];
    int f34;                    /* 0x34 */
} Tempo;

typedef struct {
    unsigned char f00;          /* 0x00 */
    unsigned char pad_01;
    unsigned char f02;          /* 0x02 */
    unsigned char f03;          /* 0x03 */
    unsigned char f04;          /* 0x04 */
    char pad_05[0x03];
    int f08;                    /* 0x08 */
    char pad_0C[0x0C];
    unsigned short f18;         /* 0x18 */
    char pad_1A[0x0A];
    unsigned short f24;         /* 0x24 */
    unsigned short f26;         /* 0x26 */
    unsigned short f28;         /* 0x28 */
    char pad_2A[0x18];
    unsigned short f42;         /* 0x42 */
} Event;

typedef struct { int t00; int t04; int t08; } Tone;

extern Song D_00281AC0;
extern Voice D_0027CCC0[48];
extern Tempo D_0027F740;
extern Tone D_0027C6C0[];
extern unsigned short D_00242630[];

extern int func_001176E0(Event *ev);
extern int func_00117428(int a, int b, int c);
extern int func_00117BA0(int a0, int a1);
extern int func_00117918(int a0, int a1, int a2, int a3, int a4);
extern int func_001157F0(int a0, int a1, int a2, int a3);
extern int func_001179E0(int idx, Event *ev);

int func_00115850(Event *ev)
{
    int slot;
    int key;
    int t;
    int param;
    unsigned long mask;

    if (ev->f03 == 0) {
        func_001176E0(ev);
        return 0;
    }

    slot = ev->f02 - D_00281AC0.f00[6];
    if (slot < 0) {
        ev->f08 += 4;
        return 0;
    }

    D_00281AC0.f04 += slot * 16;
    D_00281AC0.f0C[0xA] = 0x40;
    key = func_00117428(D_00281AC0.f04[0], D_00281AC0.f04[1], ev->f24);
    if (key == -1) {
        ev->f08 += 4;
        return 0;
    }

    mask = (unsigned long)1 << key;

    if (D_00281AC0.f04[0xF] & 1) {
        D_0027CCC0[key].f08 = 0;
        D_0027CCC0[key].f0C = 1;
    } else {
        D_0027CCC0[key].f08 = 1;
        D_0027CCC0[key].f0C = 0;
    }

    D_0027CCC0[key].f00 = 1;
    D_0027CCC0[key].f02 = ev->f02;
    D_0027CCC0[key].f04 = ev->f00 & 0xF;
    D_0027CCC0[key].f06 = ev->f18;
    D_0027CCC0[key].f0A = D_0027F740.f34;
    D_0027CCC0[key].f0E = slot;
    D_0027CCC0[key].f12 = 0;
    D_0027CCC0[key].f18 = 0;
    D_0027CCC0[key].f1A = 2;
    D_0027CCC0[key].f1C = 0;
    D_0027CCC0[key].f1E = D_00281AC0.f04[1];
    D_0027CCC0[key].f20 = D_00281AC0.f04[0];
    D_0027CCC0[key].f22 = ev->f24;
    D_0027CCC0[key].f24 = ev->f28;
    D_0027CCC0[key].f26 = ev->f26;
    D_0027CCC0[key].f28 = D_00281AC0.f0C[0xE];
    D_0027CCC0[key].f2A = D_00281AC0.f00[1];
    D_0027CCC0[key].f2C = D_00281AC0.f18[ev->f03 + 2];
    D_0027CCC0[key].f2E = D_00281AC0.f04[0xB];
    D_0027CCC0[key].f30 = D_00281AC0.f08[0];
    D_0027CCC0[key].f32 = D_00242630[func_00117BA0(4, 0) >> 2];
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
    D_0027CCC0[key].f4C = D_00281AC0.f04[0xC];
    D_0027CCC0[key].f4E = 0x78;
    D_0027CCC0[key].f62 = 0;
    D_0027CCC0[key].f66 = 0;
    D_0027CCC0[key].f68 = 0;

    if (D_00281AC0.f04[0xF] & 0x20) {
        if (D_00281AC0.f04[0xF] & 0x40) {
            D_0027CCC0[key].f10 = D_00281AC0.f00[5];
        } else {
            D_0027CCC0[key].f10 = D_00281AC0.f04[0xE];
        }
        D_0027CCC0[key].f14 = 1;
        D_0027CCC0[key].f16 = 0x7F;
    } else {
        D_0027CCC0[key].f14 = 0;
    }

    if (D_00281AC0.f04[0xF] & 0x10) {
        param = D_00281AC0.f00[4];
    } else {
        param = D_00281AC0.f04[0xD];
    }
    t = func_00117918(D_00281AC0.f04[2], ev->f02, (signed char)D_00281AC0.f04[3],
                      D_00281AC0.f0C[0xA], param);
    func_001157F0(6, key, t * 44100 / 48000, 0);

    t = func_001179E0(key, ev);
    func_001157F0(1, key, t >> 16, t & 0xFFFF);

    func_001157F0(5, key, (*(unsigned short *)(D_00281AC0.f04 + 4) + D_0027C6C0[ev->f24].t08) << 3, 0);

    func_001157F0(3, key, *(unsigned short *)(D_00281AC0.f04 + 6), *(unsigned short *)(D_00281AC0.f04 + 8));

    D_0027F740.f20 |= mask;
    if (ev->f42 == 1) {
        D_0027F740.f00 |= mask;
    } else if (D_00281AC0.f04[0xF] & 0x80) {
        D_0027F740.f00 |= mask;
    } else {
        D_0027F740.f00 &= ~mask;
    }

    if (D_00281AC0.f04[0xF] & 2) {
        D_0027F740.f10 |= mask;
        func_001157F0(0x33, 8, key, D_00281AC0.f04[2]);
    } else {
        D_0027F740.f10 &= ~mask;
    }

    D_0027F740.f34++;
    D_00281AC0.f04 -= slot * 16;
    ev->f08 += 4;
    return 0;
}

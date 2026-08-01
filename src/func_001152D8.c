// func_001152D8 — byte-identical match.
//
// Parked as a NEARMISS on a measurement taken through a scratch harness that
// does NOT run inject_relocs.py, unlike build.py's canonical `expected` path.
// That omission under-reports every relocation-class residual. Re-measured
// through the canonical pipeline: 100.0%.
// COMPILER: eegcc
// CFLAGS: -O2

//
// SEMANTICS: the sequencer's per-frame tick (this whole 0x115xxx-0x119xxx region
// is ee-gcc SDK-style sound-driver code, not mwcc game code). Walks the 0x30
// tracks of D_0027E0C0 (stride 0x78). A track only runs when it is either
// (state@0x2E == 1, word@0x30 == 1, half@0x34 == 0) or
// (state@0x2E == 0, word@0x30 == 0x10000, half@0x34 == 1). While the delta-time
// accumulator at +0x20 is <= 0 it pulls events with func_00117088 and dispatches
// on the MIDI-style status nibble p[0] & 0xF0: 0x80/0x90/0xA0 (note off / note on /
// aftertouch), 0xB0 (controller -- a 99-entry jump table indexed by p[2] - 1, with
// live cases 1, 2, 6, 7, 0xA, 0x40, 0x41, 0x60, 0x62, 0x63), 0xC0 (program change),
// 0xE0 (pitch bend) and 0xF0 (meta: 0x2F end-of-track, 0x51 set-tempo). Controller
// 0x0A is handled inline: if the global flag at D_0027F740+0x38 is 1 it copies one
// byte through D_00281AC0 and advances the read cursor p+8 by 3, else by 6, else it
// defers to func_00118418. After the event loop the accumulator is decremented by
// the tick at +0x1C and a pending restart at +0x38 is applied. Finally the four
// pending 24-bit register images in D_0027F740 (+0x10/+0x00/+0x20/+0x28) are flushed
// to the hardware via func_001157F0 (ids 0xD/0xC/0xA/0xB, split into low 24 bits and
// bits 24..47) and func_001191F0 kicks the DMA using the double-buffer index at +0x3C,
// which is then toggled.
//
// MATCH NOTES (s86): the two `volatile` qualifiers in the tail are load-bearing. Without
// them ee-gcc proves g+0x40 and g+0x3C cannot alias and hoists the 0x3C load above the
// 0x40 store, shifting the whole restore/body interleave by one slot (98.53 vs 99.997).

extern unsigned char D_0027E0C0[];
extern unsigned char D_0027F740[];
extern unsigned char D_0027F7C0[];
extern unsigned char D_002817C0[];
extern unsigned char D_00281AC0[];

extern void func_00115850(unsigned char *);
extern void func_00115E50(unsigned char *);
extern void func_00116598(void);
extern void func_00116DB8(unsigned char *);
extern int func_00117088(unsigned char *);
extern void func_001177E8(unsigned char *);
extern void func_00117C28(unsigned char *);
extern void func_00117CB0(unsigned char *);
extern void func_00117D18(unsigned char *);
extern void func_00117D70(unsigned char *);
extern void func_00117EE8(unsigned char *);
extern void func_00118078(unsigned char *);
extern void func_001181B0(unsigned char *);
extern void func_00118418(unsigned char *);
extern void func_001186B0(unsigned char *);
extern void func_00118790(unsigned char *);
extern void func_00118828(unsigned char *);
extern void func_00118BA0(unsigned char *);
extern void func_00118C00(unsigned char *);
extern void func_00118CF8(unsigned char *);
extern void func_00118E60(unsigned char *);
extern void func_00118EC0(unsigned char *);
extern void func_001157F0(int, int, int, int);
extern void func_001191F0(int, int, unsigned char *, unsigned char *, int, int);

void func_001152D8(void)
{
    unsigned char *p;
    unsigned char *g;
    unsigned long long w0;
    unsigned long long w1;
    unsigned long long w2;
    unsigned long long w3;
    unsigned long long u;
    int i;

    p = D_0027E0C0;
    func_00118EC0(D_0027E0C0);

    for (i = 0; i < 0x30; i++) {
        if ((*(unsigned short *)(p + 0x2E) == 1 && *(int *)(p + 0x30) == 1 &&
             *(unsigned short *)(p + 0x34) == 0) ||
            (*(unsigned short *)(p + 0x2E) == 0 && *(int *)(p + 0x30) == 0x10000 &&
             *(unsigned short *)(p + 0x34) == 1)) {
            if (*(int *)(p + 0x20) <= 0) {
                for (;;) {
                    if (func_00117088(p) != 0) {
                        switch (p[0] & 0xF0) {
                        case 0xF0:
                            switch (p[2]) {
                            case 0x2F:
                                func_00117C28(p);
                                goto tail;
                            case 0x51:
                                func_00117CB0(p);
                                break;
                            }
                            break;
                        case 0xC0:
                            func_00117D18(p);
                            break;
                        case 0xB0:
                            switch (p[2]) {
                            case 0x01:
                                func_00117D70(p);
                                break;
                            case 0x02:
                                func_00117EE8(p);
                                break;
                            case 0x06:
                                func_00118828(p);
                                break;
                            case 0x07:
                                func_001181B0(p);
                                break;
                            case 0x0A: {
                                unsigned char *m = D_0027F740;

                                if (*(unsigned short *)(m + 0x38) == 1) {
                                    if (*(unsigned short *)(p + 0x30) == 1) {
                                        unsigned char *c = D_00281AC0;

                                        *(*(unsigned char **)(c + 0xC) + 4) =
                                            *(unsigned char *)(*(int *)(p + 8) + *(int *)(c + 0x14) + 2);
                                        *(int *)(p + 8) = *(int *)(p + 8) + 3;
                                    } else {
                                        *(int *)(p + 8) = *(int *)(p + 8) + 6;
                                    }
                                } else {
                                    func_00118418(p);
                                }
                                break;
                            }
                            case 0x40:
                                func_001186B0(p);
                                break;
                            case 0x41:
                                func_00118078(p);
                                break;
                            case 0x60:
                                func_00118790(p);
                                func_00118E60(p);
                                if (*(unsigned short *)(p + 0x3C) == 1) {
                                    *(int *)(p + 0x20) = 0;
                                }
                                goto tail;
                            case 0x62:
                                func_00118BA0(p);
                                break;
                            case 0x63:
                                func_00118C00(p);
                                break;
                            }
                            break;
                        case 0xE0:
                            func_00118CF8(p);
                            break;
                        case 0x80:
                            func_001177E8(p);
                            break;
                        case 0xA0:
                            func_00115850(p);
                            break;
                        case 0x90:
                            func_00115E50(p);
                            break;
                        }
                    }
                    func_00118E60(p);
                    if (*(int *)(p + 0x20) > 0) {
                        break;
                    }
                }
            }

        tail:
            if (*(unsigned short *)(p + 0x54) != 0 || *(unsigned short *)(p + 0x34) == 1) {
                *(int *)(p + 0x20) = *(int *)(p + 0x20) - *(int *)(p + 0x1C);
            }
            if (*(unsigned short *)(p + 0x38) != 0) {
                unsigned char ch = p[0x3A];

                p[0] = ch;
                p[1] = ch;
                *(int *)(p + 8) = *(int *)(p + 0x14);
                *(unsigned short *)(p + 0x38) = 0;
                if (*(unsigned short *)(p + 0x3C) != 0) {
                    *(unsigned short *)(p + 0x3C) = 0;
                    *(unsigned short *)(p + 0x34) = 1;
                } else {
                    *(unsigned short *)(p + 0x3E) = 0;
                    *(unsigned short *)(p + 0x30) = 1;
                    *(int *)(p + 0x20) = 0;
                }
            }
        }

        func_00116DB8(p);
        p += 0x78;
    }

    func_00116598();
    g = D_0027F740;

    w0 = *(unsigned long long *)(g + 0x10);
    if (*(unsigned long long *)(g + 0x18) != w0) {
        func_001157F0(0xD, 0, w0 & 0xFFFFFF, (w0 >> 24) & 0xFFFFFF);
        w0 = *(unsigned long long *)(g + 0x10);
        *(unsigned long long *)(g + 0x18) = w0;
    }
    u = *(unsigned long long *)(g + 8);
    w1 = *(unsigned long long *)(g + 0);
    if (u != w1) {
        func_001157F0(0xC, 0, w1 & 0xFFFFFF, (w1 >> 24) & 0xFFFFFF);
        w1 = *(unsigned long long *)(g + 0);
        *(unsigned long long *)(g + 8) = w1;
    }
    w2 = *(unsigned long long *)(g + 0x20);
    if (w2 != 0) {
        func_001157F0(0xA, 0, w2 & 0xFFFFFF, (w2 >> 24) & 0xFFFFFF);
        *(unsigned long long *)(g + 0x20) = 0;
    }
    w3 = *(unsigned long long *)(g + 0x28);
    if (w3 != 0) {
        func_001157F0(0xB, 0, w3 & 0xFFFFFF, (w3 >> 24) & 0xFFFFFF);
        *(unsigned long long *)(g + 0x28) = 0;
    }

    func_001191F0(0x64, 1, D_0027F7C0 + (*(int *)(g + 0x3C) << 12), D_002817C0,
                  *(int *)(g + 0x40) << 4, 0x200);
    *(volatile int *)(g + 0x40) = 0;
    *(volatile int *)(g + 0x3C) = (*(volatile int *)(g + 0x3C) + 1) & 1;
}

// NEARMISS func_00116598  (vram 0x00116598, 0x820 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.80% via ee-gcc 2.9-991111-01 (-O2); .text size 0x820 matches. Matching wave
// 2026-10-27 (was 62.35%): rewritten with typed Voice D_0027CCC0[48] / Seq D_0027E0C0[48] /
// Song / Tempo records; logic fixes read from the original: the pan restarts from f4E before
// the f1A == 1 vibrato step (f02 only feeds the first argument), the >= arms come first, the
// f1A == 2 f58 == f5A test has its clear arm first, f62 / f54 / f5C are masked in place (no
// separate zero store), the loop counter and the 0xFF / 480 divisions are unsigned, and the
// arm temporaries are block-scoped (one shared `n` forced extra registers everywhere).
// Remaining differences: one instruction plus its register colouring: in the tempo-60 /
// length-120 envelope step the original ORs the held `f3C & 0xFFF` register into the new
// counter, while our build folds it to the known constant 0x78 (gcc records the equality
// from the compare); every spelling of that compare against a literal folds it.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// Per-frame voice update. For each of the 48 voices that is active (f00 == 1), not in
// state 3 (f1A) and bound to a channel record (f06 < 48) of D_0027E0C0:
//  - if a key-on (f14), a pan slide (f44) or the channel's pending flag (+0x50) is set and
//    the channel is not paused (+0x2E, +0x30 zero): computes the pitch inputs. For key-on
//    voices the 0x3C envelope word (low 12 bits length, top 4 bits repeat count) advances
//    f12; the channel's tone (D_0027C6C0 row +4) gives the Song envelope tables at +0x1C /
//    +0x24 and the level byte; f1A == 1 adds the f38/f3A vibrato to the pan (f4E) and the
//    detune; the volume comes from the level and f16. A pan slide (f44, count f62 of f64,
//    direction/amount f60) moves the pan, clamps it to 12..243, stores it in the channel
//    (+0x58) and, when finished, into f4E. Then pushes the pitch (func_00117918, scaled
//    441/480) as command 6.
//  - if f1A == 2 and a volume slide (f46: f50 -> f52 into f2C) or a second slide (f48:
//    f58 -> f5A into f4C, with f32 = D_00242630[f4C >> 2]) or the channel's +0x52 flag is
//    set: steps the slides through func_00119080 and pushes the volume pair
//    (func_001179E0) as command 1.
// Finally clears the +0x50 / +0x52 pending flags of all 48 channel records.

typedef struct {
    unsigned short f00, f02, f04, f06, f08, f0A, f0C, f0E;
    unsigned short f10, f12, f14, f16, f18, f1A, f1C, f1E;
    unsigned short f20, f22, f24, f26, f28, f2A, f2C, f2E;
    unsigned short f30, f32, f34;
    short f36;
    unsigned short f38, f3A, f3C, f3E;
    unsigned short f40, f42, f44, f46, f48, f4A, f4C, f4E;
    unsigned short f50, f52, f54, f56, f58, f5A, f5C, f5E;
    unsigned short f60, f62, f64, f66, f68;
} Voice;                        /* 0x6A */

typedef struct {
    unsigned char pad00[0x24];
    unsigned short f24;         /* 0x24 */
    unsigned char pad26[0x2E - 0x26];
    unsigned short f2E;         /* 0x2E */
    unsigned short f30;         /* 0x30 */
    unsigned char pad32[0x44 - 0x32];
    unsigned short f44;         /* 0x44 */
    unsigned char pad46[0x50 - 0x46];
    unsigned short f50;         /* 0x50 */
    unsigned short f52;         /* 0x52 */
    unsigned char pad54[0x58 - 0x54];
    unsigned short f58;         /* 0x58 */
    unsigned char pad5A[0x78 - 0x5A];
} Seq;

typedef struct {
    unsigned char pad00[0x10];
    unsigned char *f10;         /* 0x10 */
    unsigned char pad14[0x1C - 0x14];
    unsigned char *f1C;         /* 0x1C */
    unsigned char pad20[0x24 - 0x20];
    unsigned short *f24;        /* 0x24 */
} Song;

typedef struct {
    unsigned char pad00[0x3A];
    unsigned short f3A;
} Tempo;

extern Voice D_0027CCC0[48];
extern Seq D_0027E0C0[48];
extern unsigned char *D_0027C6C0[][3];
extern Song D_00281AC0;
extern Tempo D_0027F740;
extern unsigned short D_00242630[];

extern int func_00117918(int a0, int a1, int a2, int a3, int a4);
extern int func_001157F0(int a0, int a1, int a2, int a3);
extern int func_001179E0(int idx, Seq *e);
extern int func_00119080(int a, int b, int c, int d);

void func_00116598(void)
{
    Seq *e = D_0027E0C0;
    unsigned int i;
    unsigned short ah, ax, aw, af;
    short ay;
    unsigned int shade;
    unsigned short cnt;
    int rate;
    int low;
    unsigned char *ptr;
    unsigned char *end;
    int res;

    for (i = 0; i < 48; i++) {
        if (D_0027CCC0[i].f00 == 1 && D_0027CCC0[i].f1A != 3 && D_0027CCC0[i].f06 < 48) {
            e += D_0027CCC0[i].f06;

            if ((D_0027CCC0[i].f14 == 1 || D_0027CCC0[i].f44 == 1 || e->f50 == 1) &&
                e->f2E == 0 && e->f30 == 0) {
                ax = D_0027CCC0[i].f02;
                ay = D_0027CCC0[i].f36;
                aw = D_0027CCC0[i].f38;
                af = D_0027CCC0[i].f3A;
                ah = D_0027CCC0[i].f40;

                if (D_0027CCC0[i].f14 == 1 || D_0027CCC0[i].f44 == 1) {
                    if (ax >= ah) {
                        ah = 0x78 - (ax - ah);
                    } else {
                        ah = (ah - ax) + 0x78;
                    }

                    if (D_0027CCC0[i].f14 != 0) {
                        if (D_0027F740.f3A == 0x3C && (low = D_0027CCC0[i].f3C & 0xFFF) == 0x78) {
                            if ((D_0027CCC0[i].f3C & 0xF000) != 0) {
                                cnt = (D_0027CCC0[i].f3C >> 12) - 1;
                                rate = low | (cnt << 12);
                                D_0027CCC0[i].f3C = rate;
                                D_0027CCC0[i].f12 += rate & 0xFFF;
                            } else {
                                D_0027CCC0[i].f3C |= 0x6000;
                            }
                        } else {
                            D_0027CCC0[i].f12 += D_0027CCC0[i].f3C & 0xFFF;
                        }

                        shade = 0x80;
                        ptr = D_0027C6C0[e->f24][1];
                        if (((int)ptr & 3) == 0) {
                            end = ptr + *(int *)(ptr + 0x18);
                            D_00281AC0.f10 = ptr;
                            if (end != (unsigned char *)-1 && ((int)end & 1) == 0) {
                                D_00281AC0.f24 = (unsigned short *)end;
                                D_00281AC0.f1C = end;
                                if (D_0027CCC0[i].f12 >= 0xF0) {
                                    D_0027CCC0[i].f12 = (D_0027CCC0[i].f3C & 0xFFF) >> 1;
                                }
                                shade = D_00281AC0.f1C[D_00281AC0.f24[D_0027CCC0[i].f10 + 1] +
                                                       (D_0027CCC0[i].f12 >> 2)];
                            }
                        }

                        ax = D_0027CCC0[i].f4E;
                        if (D_0027CCC0[i].f1A == 1) {
                            if (D_0027CCC0[i].f38 >= 0x40) {
                                int n, hi, lo;
                                n = (D_0027CCC0[i].f38 - 0x40) * D_0027CCC0[i].f3A;
                                hi = n / 64;
                                lo = n / 4;
                                ax = ax + hi;
                                ay = ay + (lo - hi * 16);
                            } else {
                                int n, hi, lo;
                                n = (0x40 - D_0027CCC0[i].f38) * D_0027CCC0[i].f3A;
                                hi = n / 64;
                                lo = n / 4;
                                ax = ax - hi;
                                ay = ay + (hi * 16 - lo);
                            }
                            af = 1;
                        }

                        aw = (shade * D_0027CCC0[i].f16) / 0xFF + 0x40;
                        aw -= (D_0027CCC0[i].f16 + 1) >> 1;
                    }

                    if (D_0027CCC0[i].f44 != 0 && (D_0027CCC0[i].f62 &= 0x7FFF) != 0) {
                        D_0027CCC0[i].f62--;
                        if ((D_0027CCC0[i].f60 & 0x80) != 0) {
                            int n, v;
                            n = D_0027CCC0[i].f64 - D_0027CCC0[i].f62;
                            ax = D_0027CCC0[i].f4E -
                                 ((n * (0x100 - D_0027CCC0[i].f60)) / 10) / D_0027CCC0[i].f64;
                            v = (n * (unsigned char)(0x100 - D_0027CCC0[i].f60) * 0xC0) /
                                (D_0027CCC0[i].f64 * 0x78);
                            ay = ay - v % 16;
                        } else {
                            int n, v;
                            n = D_0027CCC0[i].f64 - D_0027CCC0[i].f62;
                            ax = D_0027CCC0[i].f4E +
                                 ((n * D_0027CCC0[i].f60) / 10) / D_0027CCC0[i].f64;
                            v = (n * D_0027CCC0[i].f60 * 0xC0) / (D_0027CCC0[i].f64 * 0x78);
                            ay = ay + v % 16;
                        }

                        if (ax <= 0xC) {
                            ax = 0xC;
                        }
                        if (ax >= 0xF3) {
                            ax = 0xF3;
                        }
                        e->f58 = ax;
                        if (D_0027CCC0[i].f62 == 0) {
                            D_0027CCC0[i].f4E = ax;
                            D_0027CCC0[i].f44 = 0;
                        }
                    }
                }

                res = func_00117918(ah, ax, ay, aw, af);
                func_001157F0(6, i, (((res * e->f44) >> 12) * 0x1B9U) / 0x1E0, 0);
            }

            if (D_0027CCC0[i].f1A == 2 &&
                (D_0027CCC0[i].f46 == 1 || D_0027CCC0[i].f48 == 1 || e->f52 == 1)) {
                if (D_0027CCC0[i].f46 == 1) {
                    if (D_0027CCC0[i].f50 != D_0027CCC0[i].f52) {
                        if ((D_0027CCC0[i].f54 &= 0x7FFF) != 0) {
                            D_0027CCC0[i].f2C = func_00119080(
                                (unsigned char)D_0027CCC0[i].f50, (unsigned char)D_0027CCC0[i].f52,
                                (unsigned char)D_0027CCC0[i].f56, (unsigned char)D_0027CCC0[i].f54);
                            D_0027CCC0[i].f54--;
                        } else {
                            D_0027CCC0[i].f2C = D_0027CCC0[i].f50;
                            D_0027CCC0[i].f46 = 0;
                        }
                    } else {
                        D_0027CCC0[i].f46 = 0;
                    }
                }

                if (D_0027CCC0[i].f48 == 1) {
                    if (D_0027CCC0[i].f58 == D_0027CCC0[i].f5A) {
                        D_0027CCC0[i].f48 = 0;
                    } else {
                        if ((D_0027CCC0[i].f5C &= 0x7FFF) != 0) {
                            D_0027CCC0[i].f4C = func_00119080(
                                (unsigned char)D_0027CCC0[i].f58, (unsigned char)D_0027CCC0[i].f5A,
                                (unsigned char)D_0027CCC0[i].f5E, (unsigned char)D_0027CCC0[i].f5C);
                            D_0027CCC0[i].f5C--;
                        } else {
                            D_0027CCC0[i].f4C = D_0027CCC0[i].f58;
                            D_0027CCC0[i].f48 = 0;
                        }
                        D_0027CCC0[i].f32 = D_00242630[D_0027CCC0[i].f4C >> 2];
                    }
                }

                res = func_001179E0(i, e);
                func_001157F0(1, i, res >> 16, (short)res);
            }

            e -= D_0027CCC0[i].f06;
        }
    }

    i = 0;
    do {
        i++;
        if (e->f50 == 1) {
            e->f50 = 0;
        }
        if (e->f52 == 1) {
            e->f52 = 0;
        }
        e++;
    } while (i < 48);
}

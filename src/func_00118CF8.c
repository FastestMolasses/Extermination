// NEARMISS func_00118CF8  (vram 0x00118CF8, 0x168 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 80.17% via ee-gcc 2.9-991111-01 (-O2). Logic and structure are faithful.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

//
// Rewritten (matching wave 2026-10-27, was 71.91%) in the spelling of the byte-matched
// twin func_00118418: typed Voice / Seq / Song records, the voice table indexed
// directly in a for (i < 48) loop with continue guards, the event data byte re-formed
// from seq->f08 + D_00281AC0.p14 at each use. The original reads f00 and f3A through a
// walking element pointer (its own register, advanced by 0x6A) and the other fields
// through the base plus a byte offset, so a pointer v is walked beside i (reading f04
// through v as well scores higher here, 80.17 vs 78.61).
// The divisor is a literal 48000: ee-gcc keeps the constant in a saved register and
// emits a real divide with its zero-trap guard (the old note that the trap proves a
// variable divisor was wrong; literal and variable compile identically).
// Remaining differences: ee-gcc keeps i * 0x6A as a runtime multiply instead of the
// original's strength-reduced byte-offset register, which shifts the saved-register
// assignment and the frame (0x70 vs 0x80); no source spelling tried (all-indexed,
// pointer-only, element pointer per iteration, unsigned i, result local, operand order)
// reproduces the original's offset induction variable.
//
// Sequencer event with a 2-byte payload: copy payload[1] into byte 0xA of the song's
// p0C block; for every voice with f04 == (seq->f00 & 0xF), f22 == seq->f24,
// f06 == seq->f18, f1A == 1 and f00 == 1: store payload[1] in f38 and call
// func_001157F0(6, i, func_00117918(f40, f02, f36, p0C[0xA], f3A) * 44100 / 48000, 0).
// Then advance seq->f08 by 2.

typedef struct {                /* 0x6A */
    unsigned short f00;         /* 0x00 */
    unsigned short f02;         /* 0x02 */
    unsigned short f04;         /* 0x04 */
    unsigned short f06;         /* 0x06 */
    unsigned char pad08[0x1A - 0x08];
    unsigned short f1A;         /* 0x1A */
    unsigned char pad1C[0x22 - 0x1C];
    unsigned short f22;         /* 0x22 */
    unsigned char pad24[0x36 - 0x24];
    short f36;                  /* 0x36 */
    unsigned short f38;         /* 0x38 */
    unsigned short f3A;         /* 0x3A */
    unsigned char pad3C[0x40 - 0x3C];
    unsigned short f40;         /* 0x40 */
    unsigned char pad42[0x6A - 0x42];
} Voice;

typedef struct {
    unsigned char f00;          /* 0x00 */
    unsigned char pad01[0x08 - 0x01];
    int f08;                    /* 0x08 */
    unsigned char pad0C[0x18 - 0x0C];
    int f18;                    /* 0x18 */
    unsigned char pad1C[0x24 - 0x1C];
    unsigned short f24;         /* 0x24 */
} Seq;

typedef struct {
    int f00;                    /* 0x00 */
    int f04;                    /* 0x04 */
    int f08;                    /* 0x08 */
    unsigned char *p0C;         /* 0x0C */
    int f10;                    /* 0x10 */
    unsigned char *p14;         /* 0x14 */
} Song;

extern Voice D_0027CCC0[48];
extern Song D_00281AC0;

extern int func_00117918(int a0, int a1, int a2, int a3, int a4);
extern int func_001157F0(int a0, int a1, int a2, int a3);
void func_00118CF8(Seq *seq)
{
    Voice *v = D_0027CCC0;
    int i;
    int r;

    D_00281AC0.p0C[0xA] = D_00281AC0.p14[seq->f08 + 1];
    for (i = 0; i < 48; i++, v++) {
        if (v->f04 != (seq->f00 & 0xF))
            continue;
        if (D_0027CCC0[i].f22 != seq->f24)
            continue;
        if (D_0027CCC0[i].f06 != seq->f18)
            continue;
        if (D_0027CCC0[i].f1A != 1)
            continue;
        if (v->f00 != 1)
            continue;
        D_0027CCC0[i].f38 = D_00281AC0.p14[seq->f08 + 1];
        r = func_00117918(D_0027CCC0[i].f40, D_0027CCC0[i].f02, D_0027CCC0[i].f36,
                          D_00281AC0.p0C[0xA], v->f3A);
        func_001157F0(6, i, r * 44100 / 48000, 0);
    }
    seq->f08 += 2;
}

// NEARMISS func_00117088  (vram 0x00117088, 0x22C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 84.37% via ee-gcc 2.9-991111-01 (-O2). Matching wave 2026-10-28 (was 55.16%, then 82.88%):
// rewritten with typed Event / Song / Bank / Tone records; fixes read from the original:
// the event's data pointer (+0x0C) and cursor (+0x08) are read into locals once; a
// running-status byte (no 0x80 bit) steps the cursor back and re-reads it from the event;
// both arms share a single `return 1`; the f04 program pointer of the f32 == 1 arm is the
// bank's array at +8, held in a local set at the top of that arm (this keeps `bank + 8`
// an early separate value as in the original instead of folding the +8 into the sum).
// Remaining differences (wave 2026-10-28, agent wZ1): the other arm still folds its
// `bank->f10 + 8` (the original adds 8 to the loaded f10 early and adds `index + bank`
// to it; local / cast spellings of that regress the colouring) and adds bank + f10 in
// the opposite operand order; plus the register colouring that follows (the event
// pointer lands in a3 instead of t3; the f10 == -1 guard becomes a branch-likely).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2

// Sequencer event decode (leaf). Points the shared Song context D_00281AC0 at the
// event's sound bank (D_0027C6C0[ev->f24].t04): f14 = event data, f10 = bank, f18 / f20 =
// the bank's tables at its +0x14 offset and at the +0x10 offset of entry ev->f32. Reads
// the status byte at data[cursor] (a byte without the 0x80 bit reuses the running status
// ev->f01 and steps the cursor back) and the three following bytes into ev->f02..f04.
// Mode 1 (ev->f32 == 1): selects the program from the bank's +0x24 table through the
// 16-bit index at f20 + 0x312 + 2 * ev->f04, sets f08 / f0C from the bank's +0x20 table
// and ev->f18. Otherwise: channel ev->f00 & 0xF; below 0xA0 the channel's program index
// (data[ch * 16 + 0x12]) is checked (index entry 0xFFFF, beyond the table count, or the
// bank's +0x10 offset -1: skip 3 bytes, return 0); then f00 / f04 come from the bank's
// +0x10 table and f08 / f0C from the event data. Returns 1.

typedef struct {
    int f00;                    /* 0x00 */
    int f04;                    /* 0x04 */
    unsigned char f08[8];       /* 0x08 */
    int f10;                    /* 0x10 */
    int f14;                    /* 0x14 */
    char pad_18[0x08];
    int f20;                    /* 0x20 */
    int f24;                    /* 0x24 */
} Bank;

typedef struct {
    unsigned char *f00;         /* 0x00 */
    unsigned char *f04;         /* 0x04 */
    unsigned char *f08;         /* 0x08 */
    unsigned char *f0C;         /* 0x0C */
    Bank *f10;                  /* 0x10 */
    unsigned char *f14;         /* 0x14 */
    unsigned char *f18;         /* 0x18 */
    unsigned char *f1C;         /* 0x1C */
    unsigned char *f20;         /* 0x20 */
} Song;

typedef struct {
    unsigned char f00;          /* 0x00 */
    unsigned char f01;          /* 0x01 */
    unsigned char f02;          /* 0x02 */
    unsigned char f03;          /* 0x03 */
    unsigned char f04;          /* 0x04 */
    char pad_05[0x03];
    int f08;                    /* 0x08 */
    unsigned char *f0C;         /* 0x0C */
    char pad_10[0x08];
    int f18;                    /* 0x18 */
    char pad_1C[0x08];
    unsigned short f24;         /* 0x24 */
    char pad_26[0x0C];
    unsigned short f32;         /* 0x32 */
} Event;


typedef struct { int t00; Bank *t04; int t08; } Tone;

extern Song D_00281AC0;
extern Tone D_0027C6C0[];

int func_00117088(Event *ev)
{
    unsigned char *data;
    int pos;
    Bank *buf;
    unsigned char b;
    int ch;
    unsigned char *prog;

    data = ev->f0C;
    pos = ev->f08;
    D_00281AC0.f14 = data;
    buf = D_0027C6C0[ev->f24].t04;
    D_00281AC0.f10 = buf;
    D_00281AC0.f18 = (unsigned char *)buf + buf->f14;
    D_00281AC0.f20 = (unsigned char *)buf + *(int *)((unsigned char *)buf + ev->f32 * 16 + 0x10);

    b = data[pos];
    if (b & 0x80) {
        ev->f01 = b;
        ev->f00 = D_00281AC0.f14[pos];
    } else {
        ev->f00 = ev->f01;
        ev->f08 = pos - 1;
        pos = ev->f08;
    }
    ev->f02 = D_00281AC0.f14[pos + 1];
    ev->f03 = D_00281AC0.f14[pos + 2];
    ev->f04 = D_00281AC0.f14[pos + 3];

    if (ev->f32 == 1) {
        prog = buf->f08;
        D_00281AC0.f00 = ((unsigned char *)buf + D_00281AC0.f10->f24) +
            *(unsigned short *)(D_00281AC0.f20 + D_00281AC0.f14[pos + 3] * 2 + 0x312);
        D_00281AC0.f08 = (unsigned char *)buf + D_00281AC0.f10->f20;
        D_00281AC0.f0C = D_00281AC0.f08 + (ev->f18 * 16 + 0x10);
        D_00281AC0.f04 = prog +
            (*(unsigned short *)(D_00281AC0.f20 + D_00281AC0.f14[pos + 3] * 2 + 0x312) +
             D_00281AC0.f10->f24);
    } else {
        ch = (ev->f00 & 0xF) * 16;
        if (ev->f00 < 0xA0) {
            if (*(unsigned short *)(D_00281AC0.f20 + D_00281AC0.f14[ch + 0x12] * 2 + 2) == 0xFFFF ||
                *(unsigned short *)D_00281AC0.f20 < D_00281AC0.f14[ch + 0x12] ||
                D_00281AC0.f10->f10 == -1) {
                ev->f08 = pos + 3;
                return 0;
            }
        }
        D_00281AC0.f00 = (D_00281AC0.f10->f10 + (unsigned char *)buf) +
            *(unsigned short *)(D_00281AC0.f20 + D_00281AC0.f14[ch + 0x12] * 2 + 2);
        D_00281AC0.f0C = data + (ch + 0x10);
        D_00281AC0.f08 = data;
        D_00281AC0.f04 = (unsigned char *)buf + D_00281AC0.f10->f10 + 8 + *(unsigned short *)(D_00281AC0.f20 + D_00281AC0.f14[ch + 0x12] * 2 + 2);
    }
    return 1;
}

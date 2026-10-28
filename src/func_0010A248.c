// NEARMISS func_0010A248  (vram 0x0010A248, 0x4C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 94.26% via ee-gcc 2.9-991111-01 (-O2 -fno-schedule-insns), matching wave 2026-10-27
// (was 57.26%: the old C used a do/while(0) block whose loop notes pinned the epilogue's
// return-address reload late; the slot table is now the struct used by the MPEG decoder files).
// Remaining differences: the original stores slot 0's buffer, then the active index, then the
// slot addresses in field order, with the first store after the three address sums; this C
// gets the same instruction set and size but a different store order and a swapped pair of
// argument registers (base pointer vs the high half of the work-buffer address). Every honest
// ordering of the five stores (120) and the sums/base placement (36) was swept, with and without
// the first scheduling pass; a 9-minute permuter run only reached the right registers through
// dummy do/while(0) blocks, which move the return-address reload instead.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s,
// NOT from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff
// unit / excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: eegcc
// CFLAGS: -O2 -fno-schedule-insns

/* MPEG decoder buffer setup: switch the IPU control bit via func_00105148(1), then
 * carve the work buffer (D_002412E8) into the two decoder slots of D_002418F8
 * (slot 0 at +0 / +0x1800, slot 1 at +0x1B00 / +0x3300) and make slot 0 active. */

extern void func_00105148(unsigned int a0);

struct mb_buf {
    unsigned int w0;
    unsigned int addr;
    int pad[77];
    int done;
};
struct mb_bufs {
    struct mb_buf e[2];   /* 2 x 0x140-byte slots */
    int cur;              /* active-slot index at +0x280 */
};

extern unsigned int D_002412E8;      /* decoder work-buffer address */
extern struct mb_bufs D_002418F8;

void func_0010A248(void) {
    unsigned int v0;
    unsigned int a, b, c;
    struct mb_bufs *p;
    func_00105148(1);
    p = &D_002418F8;
    v0 = D_002412E8;
    a = v0 + 0x3300;
    b = v0 + 0x1800;
    c = v0 + 0x1B00;
    p->e[0].addr = b;
    p->e[1].addr = a;
    p->e[1].w0 = c;
    p->e[0].w0 = v0;
    p->cur = 0;
}

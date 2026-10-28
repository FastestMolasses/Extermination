// NEARMISS func_001D2300  (vram 0x001D2300, 0x280 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.62% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and
// STRUCTURE are faithful (body rewritten in matching wave 2026-10-27, was 80.44%: D_00810E88
// and D_008106C4 are reached absolute, so they are unsized externs; D_00810E88 is the
// volatile field counter; the later D_00275670 reads are fresh global reads, not a cached
// pointer; both texture blocks use the same slot * 0x60800 stride; the old "mode" doubling
// was not in the original). Remaining differences: only the two matrix-setup calls'
// argument setup — the original computes D_00275674 + slot*0x190 into a0 before reading
// the field counter (first call) and colours the 1 - counter temporaries v0/v1 directly
// into a0 for arg0 (both calls); declaration/statement order, inline vs local flip, pointer
// locals, volatile/SPAD-extern spellings and callee parameter types did not move it.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

//
// Per-frame render/VIF display-list setup for the current buffer slot
// (D_00275670->field_9C, sub-object base D_00275674 + slot*0x190). Appends a
// tag block at the channel cursor D_00275670->buf14 (byte+3=0x30,
// word+4=D_00275674, half+0=1, advance +0x10), calls func_001D1F80(1,0,7),
// sets up two matrices via func_001015A8/func_00101810 into slot+0x40/slot+0xC0
// with the scratchpad screen offsets and the 16-bit toggle (1 - D_00810E88),
// refreshes the display-list pointer (func_001D2110), and appends command 0x19
// at slot+0x20. Appends command 8 at D_00275674+0x420 (then func_001D2830(3,0))
// if func_001D2910(3), else at D_00275674+0x3A0. Unless D_008106C4 is set:
// if func_001D2910(4) is clear and func_001D2910(0x20) is set, calls
// func_001E0DF0; then pushes the block at D_0028F700 + slot*0x60800 + 0x8000
// with ptr10, and the ptr0/ptr4 pair. Always pushes the block at D_0028F700 +
// slot*0x95760 + 0xC9000 with buf14. If D_008106C4 is set, pushes the
// slot*0x60800 block with ptr10 and the ptr0/ptr4 pair after it. Finally
// dispatches via func_001D21E0.

typedef struct {
    char *ptr0;
    char *ptr4;
    char pad_8[0x08];
    char *ptr10;
    char *buf14;
    char pad_18[0x84];
    int field_9C;
} State;

extern State *D_00275670;
extern char *D_00275674;
extern char D_0028F700[];
extern volatile short D_00810E88[];
extern unsigned char D_008106C4[];

extern void func_001D1F80(int ch, int a, int b);
extern void func_001015A8(char *a0, int a1, int a2, int a3);
extern void func_00101810(char *a0, int a1, int a2, int a3);
extern void func_001D2110(void);
extern void func_001D2130(char *a0);
extern void func_001D2160(char *a0);
extern void func_001D2180(char *a0, short a1);
extern void func_001D21E0(void);
extern int func_001D2830(int id, int on);
extern int func_001D2910(int id);
extern void func_001E0DF0(void);

void func_001D2300(void) {
    State *st;
    int slot;
    int off;
    short flip;

    st = D_00275670;
    slot = st->field_9C;
    {
        char *ctx = D_00275674;
        st->buf14[3] = 0x30;
        *(char **)(st->buf14 + 4) = ctx;
        *(short *)st->buf14 = 1;
        st->buf14 = st->buf14 + 0x10;
    }

    func_001D1F80(1, 0, 7);

    off = slot * 0x190;
    flip = 1 - D_00810E88[0];
    func_001015A8(D_00275674 + off + 0x40, *(volatile short *)0x70003B70, *(volatile short *)0x70003B72, flip);
    flip = 1 - D_00810E88[0];
    func_00101810(D_00275674 + off + 0xC0, *(volatile short *)0x70003B70, *(volatile short *)0x70003B72, flip);
    func_001D2110();
    func_001D2180(D_00275674 + off + 0x20, 0x19);

    if (func_001D2910(3) != 0) {
        func_001D2180(D_00275674 + 0x420, 8);
        func_001D2830(3, 0);
    } else {
        func_001D2180(D_00275674 + 0x3A0, 8);
    }

    if (D_008106C4[0] == 0) {
        if (func_001D2910(4) == 0) {
            if (func_001D2910(0x20) != 0) {
                func_001E0DF0();
            }
        }
        func_001D2130(D_0028F700 + slot * 0x60800 + 8 + 0x7FF8);
        func_001D2160(D_00275670->ptr10);
    }
    if (D_008106C4[0] == 0) {
        func_001D2130(D_00275670->ptr0);
        func_001D2160(D_00275670->ptr4);
    }

    func_001D2130(D_0028F700 + slot * 0x95760 + 0xC9000);
    func_001D2160(D_00275670->buf14);

    if (D_008106C4[0] != 0) {
        func_001D2130(D_0028F700 + slot * 0x60800 + 8 + 0x7FF8);
        func_001D2160(D_00275670->ptr10);
        func_001D2130(D_00275670->ptr0);
        func_001D2160(D_00275670->ptr4);
    }

    func_001D21E0();
}

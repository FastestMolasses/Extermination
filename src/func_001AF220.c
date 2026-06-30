// NEARMISS func_001AF220  (vram 0x001AF220, 0x9C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 76.87% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// constant-propagation era difference: target reloads (lbu) D_00810700/D_00810701 before the indexed sb into D_00810730; both 991202 and 233 const-fold the just-stored values into a single `sb zero, D_00810730+0xb`. Not fixable by 2.3.3 (both modern builds fold).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (mwcc 2.3.3 = 76.9%, pinned 991202 = 60.6%). Logic fully recovered.
// Saves the 0x640-byte global state block at 0x70000640 -> backup buffer
// (D_00810700), runs func_001AF2C0, restores it to 0x70000000, then patches the
// first three bytes of the block (D_00810700 = 0xB, +1 = 0, +2 = 0), writes
// D_00810701 (=0) into the table at D_00810730 indexed by D_00810700 (=0xB),
// and finally copies the patched block back to 0x70000640. block_copy(dst,src,
// len). The two 0x70000640 args differ only in which base reg holds them.
//
// WALL: a compiler-era constant-propagation difference, NOT fixable by 2.3.3.
// The target reloads the just-stored bytes (`lbu D_00810700` for the index and
// `lbu D_00810701` for the value) before the indexed `sb` into D_00810730 --
// the original 2.3.1 does not propagate the stored constants across the
// statements. Both 991202 and 233 const-fold them, emitting a single
// `sb zero, D_00810730+0xb`. Tried plain &-symbol and `[]`-array forms: both
// fold identically. Const-prop-era wall.
extern void block_copy(unsigned char *dst, unsigned char *src, int len);
extern void func_001AF2C0(void);
extern unsigned char D_00810700;
extern unsigned char D_00810701;
extern unsigned char D_00810702;
extern unsigned char D_00810730;

void func_001AF220(void) {
    block_copy((unsigned char *)0x70000640, &D_00810700, 0x640);
    func_001AF2C0();
    block_copy((unsigned char *)0x70000000, &D_00810700, 0x640);
    D_00810700 = 0xB;
    D_00810701 = 0;
    D_00810702 = 0;
    (&D_00810730)[D_00810700] = D_00810701;
    block_copy(&D_00810700, (unsigned char *)0x70000640, 0x640);
}

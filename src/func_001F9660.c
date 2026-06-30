// NEARMISS func_001F9660  (vram 0x001F9660, 0x114 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.23% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// All logic, case constants, store values, branch-likely (beql) dispatch, and single shared call site match. Sole residual: the region-compare `(...)==0x1500` is lowered as `bne a0,v1,0x88` (else-block first, empty delay slot) but the target uses `beq a0,v1,0x50` (then-block first) with the ==switc...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// Copies 0x2F0 bytes into a local buffer, then sets buf[3] based on a region key
// ((D_00810700<<8)+D_00810701) and the `key` arg, dispatching func_001F91C0(buf).
// Default (no key match) returns without calling. Single shared call site; switch
// cases written in reversed source order (mwcc reverses to ascending dispatch).
// Sole residual: the region-compare emits `bne` (else-first layout) where the
// target emits `beq` (then-first) with the ==block's first compare in the delay
// slot -- a block-ordering/branch-lowering artifact that offset-shifts the tail.
extern void block_copy(void *dst, void *src, int len);
extern void func_001F91C0(void *buf);
extern unsigned char D_00810700;
extern unsigned char D_00810701;

void func_001F9660(void *src, int key) {
    unsigned char buf[0x2F0];

    block_copy(buf, src, 0x2F0);
    if (((D_00810700 << 8) + D_00810701) == 0x1500) {
        switch (key) {
        case 0x9D:
            buf[3] = 0xA;
            break;
        case 0xA0:
            buf[3] = 0xB;
            break;
        default:
            return;
        }
    } else {
        switch (key) {
        case 0x9C:
            buf[3] = 0x2;
            break;
        case 0x9D:
            buf[3] = 0x9;
            break;
        case 0x6E:
            buf[3] = 0x1;
            break;
        case 0x72:
            buf[3] = 0xC;
            break;
        default:
            return;
        }
    }
    func_001F91C0(buf);
}

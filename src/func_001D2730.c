// NEARMISS func_001D2730  (vram 0x001D2730, 0x100 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.12% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring permutation: target colors bit->s1/flags->s0/hit->s2, mwcc picks bit->s0/flags->s2/hit->s1. Instruction selection, gp-rel addressing, switch dispatch order, scheduling and frame all match. Permuter-class.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS 95.2% (mwcc 2.3.3; pinned 991202 reaches 86.9%). Body/structure are
// fully recovered and verified; the sole residual is a register-coloring
// permutation: the target colors bit->$s1, flags->$s0, hit->$s2, whereas mwcc
// (both builds) picks bit->$s0, flags->$s2, hit->$s1. Instruction selection,
// gp-rel addressing of D_00275670, switch-dispatch order (4,32,3,1,2 then 0),
// scheduling and frame all match the target -- only the $s0/$s1/$s2 assignment
// differs. Permuter territory.
//
// Semantics: maintain a bitmask of "active" flags at D_00275670+0xC, one bit per
// channel a0 (bit = 1<<a0). hit = whether that bit was already set. For channel 0
// only, swap/copy three 0x20-byte blocks in D_00275670 (the +0xA0/+0xC0/+0x100
// banks) depending on a1 (enable) and whether the bit was already set. All other
// channels (1,2,3,4,0x20) skip the block work. Finally set or clear the bit per
// a1, store the mask back, and return whether the bit had been set.

extern int *D_00275670;
void block_copy(void *dst, void *src, int count);

int func_001D2730(int a0, int a1) {
    int *base = D_00275670;
    int flags = *(int *)((char *)base + 0xC);
    int bit = 1 << a0;
    int hit = flags & bit;

    switch (a0) {
    case 0:
        if (a1 != 0) {
            if (hit == 0) {
                block_copy((char *)base + 0xA0, (char *)base + 0xC0, 0x20);
            }
        } else if (hit != 0) {
            block_copy((char *)base + 0xC0, (char *)base + 0xA0, 0x20);
            block_copy((char *)D_00275670 + 0xA0, (char *)D_00275670 + 0x100, 0x20);
        }
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 0x20:
        break;
    }
    if (a1 != 0) {
        flags = flags | bit;
    } else {
        flags = flags & ~bit;
    }
    *(int *)((char *)D_00275670 + 0xC) = flags;
    return hit != 0;
}

// NEARMISS func_001D7FA0  (vram 0x001D7FA0, 0xC0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 97.29% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// register-save / prologue scheduling-order permutation: target saves both callee fprs (f12->f21, f13->f20) before the int arg save (a2->s1) and fills the bnez delay slot with a2->s1; mwcc emits a2->s1 first and delays f13->f20 into the slot. Body byte-identical otherwise (post-increment idiom fixe...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// NEARMISS 97.29% (mwcc 2.3.3). Body/structure fully recovered; sole residual
// is a register-save / instruction-scheduling permutation in the prologue:
// the target emits the two callee-fpr saves (f12->f21, f13->f20) first and
// drops the int save (a2->s1) into the bnez delay slot, while mwcc emits
// a2->s1 first and delays f13->f20. Not the clean-store nop; permuter territory.
//
// Slot-table allocator over the global pool D_00275670. If the live count at
// +0x214 is >= 0x20 returns -1; otherwise carves slot = base + (count<<7) + 0x1220,
// copies two records (func_00102948 to slot+0x10 from a0, slot+0x20 from a1),
// stores the two float args at slot+0/+4, assigns the slot id from the
// post-incremented allocator at +0x210 into slot+0xC, stores a2 at slot+8,
// bumps the live count at +0x214, and returns the slot id.
extern char *D_00275670;
extern void func_00102948(void *dst, void *src);

int func_001D7FA0(void *a0, void *a1, int a2, float fa, float fb) {
    char *slot;
    int idx;

    idx = *((int *) (D_00275670 + 0x214));
    if (idx >= 0x20) {
        return -1;
    }
    slot = D_00275670 + (idx << 7) + 0x1220;
    func_00102948(slot + 0x10, a0);
    func_00102948(slot + 0x20, a1);
    *((float *) (slot + 0x0)) = fa;
    *((float *) (slot + 0x4)) = fb;
    *((int *) (slot + 0xC)) = (*((int *) (D_00275670 + 0x210)))++;
    *((int *) (slot + 0x8)) = a2;
    (*((int *) (D_00275670 + 0x214)))++;
    return *((int *) (slot + 0xC));
}

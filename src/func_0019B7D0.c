// NEARMISS func_0019B7D0  (vram 0x0019B7D0, 0xE8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 92.38% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Scheduler delay-slot-fill artifact (mwcc 2.3.3 vs 2.3.1 target). 233 fills the `beqz s0` branch delay slot with a speculatively-hoisted side-effect-free `lui at,0x7000` (high-half addr for the trailing *0x700031D8=ret store) and duplicates that lui at the merge; the 2.3.1 target leaves the slot a...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 92.4% (mwcc 2.3.3). Logic fully recovered; readable C.
// Wall: scheduler delay-slot-fill artifact. mwcc 2.3.3 fills the `beqz s0`
// branch delay slot with a speculatively-hoisted `lui at, 0x7000` (the
// high-half address load for the trailing `*0x700031D8 = ret` store), and
// duplicates that lui at the merge; the 2.3.1 target leaves the slot as nop.
// That single hoist is the sole divergence — the other 4 diffs are its
// downstream consequences (extra lui + shifted branch offsets). Not fixable
// by reshaping C: `lui at` is a side-effect-free constant the scheduler is
// always free to sink into the slot. (mwcc 991202 is worse at 81.2%: it
// permutes the loop-2 register allocation, s0/s1 swapped.)
//
// Semantics: copies two input float vectors a0[0..2], a1[0..2] into the
// global block at D_70003190 (a0 -> +0x00, a1 -> +0x10), stashing a1 in a
// local; seeds three globals to 1.0f / 0 (D_70003190+0x0C, +0x1C, 0x700031D4);
// calls func_0019E280 (an acceptance/validation check). If it returns nonzero,
// ret=4 and the a1 copy is re-applied (D_70003190+0x10); otherwise 0x700031D0
// is cleared. Records the result code at 0x700031D8 and returns it.

extern float D_70003190[];
extern int func_0019E280(void);

int func_0019B7D0(float *a0, float *a1) {
    float local[4];
    int i;
    int ret = 0;

    for (i = 0; i < 3; i++) {
        float t;
        D_70003190[i] = a0[i];
        t = a1[i];
        local[i] = t;
        D_70003190[i + 4] = t;
    }

    *(float *)0x700031AC = 1.0f;
    *(float *)0x7000319C = 1.0f;
    *(int *)0x700031D4 = 0;
    local[3] = 1.0f;

    if (func_0019E280()) {
        ret = 4;
    }

    if (ret != 0) {
        for (i = 0; i < 3; i++) {
            D_70003190[i + 4] = local[i];
        }
    } else {
        *(int *)0x700031D0 = 0;
    }

    *(int *)0x700031D8 = ret;
    return ret;
}

// NEARMISS func_0019F1A0  (vram 0x0019F1A0, 0x184 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 93.20% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful.
// Remaining differences: register colouring only (every instruction in target order) plus one
// copy: the original builds the per-axis node-column induction pointer (nodes + 2*i) with an add of
// zero where this C gets a plain register copy. The original keeps the node table in $ra and the
// slab base / axis offset in s0/s1; mwcc here gives the two hoisted bounds the saved registers.
// Tried: explicit induction locals (old C, 92.27%), local declaration orders, explicit bound locals,
// block-local orders, while vs guarded do-while, pointer types (unsigned compares, wrong), 2.3.3
// permuter (2 runs, ~590 s, plateau).
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// For each of the six slab planes (mask bit i set), binary-searches the sorted node-index list
// D_70003210[i] (cnt entries) for the last node whose vertex coordinate on axis i/2 is below
// pt[axis], and stores that position (or 0) in D_70003240[i].
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

typedef struct { short v[32]; } Node;
typedef struct { float f[3]; } Vec3;
extern int D_70003210[6];
extern short D_70003240[6];

void func_0019F1A0(float *pt, unsigned int mask) {
    int i;
    Vec3 *verts;
    int cnt;
    Node *nodes;

    nodes = *(Node **)0x70003208;
    cnt = *(int *)0x7000320C;
    verts = *(Vec3 **)0x700031FC;
    for (i = 0; i < 6; i++) {
        if (mask & 1) {
            int base = D_70003210[i];
            int ax = i >> 1;
            float key = pt[ax];
            int lo = base;
            int mid = base + ((cnt >> 1) << 1);
            int hi = base + (cnt << 1);
            while (lo < mid) {
                if (verts[nodes[*(short *)mid].v[i]].f[ax] < key) {
                    lo = mid;
                } else {
                    hi = mid;
                }
                mid = lo + (((hi - lo) >> 1) & ~1);
            }
            if (!(verts[nodes[*(short *)lo].v[i]].f[ax] <= key)) {
                D_70003240[i] = 0;
            } else {
                D_70003240[i] = (lo - D_70003210[i]) >> 1;
            }
        }
        mask >>= 1;
    }
}

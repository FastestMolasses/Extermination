// NEARMISS func_00199DB0  (vram 0x00199DB0, 0x1EC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 84.55% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Early-return guard branch-sense + FP-coloring near-miss. Logic fully recovered. Residual: mwcc lowers each `if(x==0) return 0;` guard as `bnez x,cont; b end` instead of the target's single `beqz x,end`, and colors the 2.0 divisor constant into a different FP register. Restructuring to a single re...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS 84.6% (mwcc 2.3.3; pinned 991202 build 75.6%). Logic and control flow
// fully recovered; residual is the early-return guard branch sense and FP-register
// coloring of the constant 2.0 divisor, both compiler scheduling artifacts.
//
// Writes a 3-float midpoint (a "center") into the caller's vec at arg0+0,4,8 from
// whichever target descriptor is currently active. Returns 1 on success, 0 if no
// valid target. Globals: cur = *0x700031D0 (active descriptor pointer).
//   - cur == 0                 -> return 0
//   - cur == &D_700030B0       -> a special "lock-on" target:
//        sub = *0x700031D4; require sub != 0 and *0x700031D8 == 2;
//        idx = (u16 sub[0xE]) >> 8; require idx != 0xFF;
//        rec = *0x70003250 + (record table); off = rec[idx*4 + 4]; require off != 0;
//        e = *0x70003250 + off; center = (e[lo] + e[hi]) / 2 over the 3 axes
//        (lo at +0/4/8, hi at +0xC/0x10/0x14).
//   - otherwise (a poly/face record): cur holds three pairs of s16 vertex indices
//        (at +0/+2, +4/+6, +8/+0xA); the vertex table base is *0x700031FC with a
//        stride of 0xC bytes, axis-offset 0/4/8 per coordinate. Each output axis is
//        the average of the two indexed vertices' matching component.
//
// Wall (not the clean-store nop): mwcc lowers the `if(x==0) return 0;` guards as
// `bnez x, cont; b end` rather than the target's single `beqz x, end`, and colors
// the 2.0 divisor into a different FP register; structurally identical otherwise.
extern int D_700030B0;

int func_00199DB0(char *arg0) {
    char *p;
    char *q;
    int idx;
    int base;
    int off;
    int v;
    int va, vb;
    int tab;

    p = *(char **)0x700031D0;
    if (p == 0) {
        return 0;
    }
    if (p == (char *)&D_700030B0) {
        q = *(char **)0x700031D4;
        if (q == 0) {
            return 0;
        }
        if (*(int *)0x700031D8 != 2) {
            return 0;
        }
        idx = (int)*(unsigned short *)(q + 0xE) >> 8;
        if (idx == 0xFF) {
            return 0;
        }
        base = *(int *)0x70003250;
        off = *(int *)(base + idx * 4 + 4);
        if (off == 0) {
            return 0;
        }
        v = base + off;
        *(float *)(arg0 + 0) = (*(float *)(v + 0) + *(float *)(v + 0xC)) / 2.0f;
        *(float *)(arg0 + 4) = (*(float *)(v + 4) + *(float *)(v + 0x10)) / 2.0f;
        *(float *)(arg0 + 8) = (*(float *)(v + 8) + *(float *)(v + 0x14)) / 2.0f;
        return 1;
    }
    va = *(short *)(p + 0);
    vb = *(short *)(p + 2);
    tab = *(int *)0x700031FC;
    *(float *)(arg0 + 0) = (*(float *)(tab + va * 0xC) + *(float *)(tab + vb * 0xC)) / 2.0f;
    p = *(char **)0x700031D0;
    tab = *(int *)0x700031FC + 4;
    va = *(short *)(p + 4);
    vb = *(short *)(p + 6);
    *(float *)(arg0 + 4) = (*(float *)(tab + va * 0xC) + *(float *)(tab + vb * 0xC)) / 2.0f;
    p = *(char **)0x700031D0;
    tab = *(int *)0x700031FC + 8;
    va = *(short *)(p + 8);
    vb = *(short *)(p + 0xA);
    *(float *)(arg0 + 8) = (*(float *)(tab + va * 0xC) + *(float *)(tab + vb * 0xC)) / 2.0f;
    return 1;
}

// NEARMISS func_001A8DA0  (vram 0x001A8DA0, 0xDC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 87.82% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Instruction-scheduling / delay-slot-fill permutation only (register coloring s0=e,s1=p,s2=q,s3=outer already matches). Target packs beqz/lw without mwcc's inserted nops, hoists the D_00275BA0 load, and reloads the 0x70003B86 counter where mwcc caches it. Body 100% correct. Permuter-class. 233=87....
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS 87.8% (mwcc 2.3.3). Double-loop collision/interaction pass over two
// global object lists. Outer list: D_00275BB0 (base) / D_00275BB8 (count); inner
// list: D_00275BA0 (base) / D_00275BA8 (count). Runs only if both counts are
// nonzero. For each outer entry e whose flag byte e[0] has bit0 set, it walks the
// inner list using a global short countdown at 0x70003B86 (seeded from
// D_00275BA8): for each inner entry f with bit0 of f[0] set, type byte f[3]==3 and
// f[0xD]==0, it calls func_001A8CE0(e, f). The 0x70003B86 counter is decremented
// per inner iteration and re-tested, so a nested handler can shorten the inner
// walk.
//
// Body/logic + register coloring (s0=e, s1=p, s2=q, s3=outer) all match. Residual
// is pure instruction scheduling / delay-slot fill: the target packs the
// beqz/lw pair without the nop mwcc inserts, hoists the D_00275BA0 load one slot,
// and reloads the 0x70003B86 counter before the inner lw a1 where mwcc keeps it
// cached. Scheduling-class -> permuter (not the clean-store nop). 233 (87.8%)
// beats 991202 (85.6%) thanks to gp-rel small-data; pinned build is 233.
extern void func_001A8CE0(unsigned char *, unsigned char *);
extern unsigned char **D_00275BA0;
extern short D_00275BA8;
extern unsigned char **D_00275BB0;
extern short D_00275BB8;

void func_001A8DA0(void) {
    int n;
    unsigned char *e;
    unsigned char **p;
    unsigned char **q;
    unsigned char *f;

    if (D_00275BA8 == 0) return;
    n = D_00275BB8;
    if (n == 0) return;
    q = D_00275BB0;
    do {
        e = *q++;
        n--;
        if ((*e & 1) != 0) {
            p = D_00275BA0;
            *(short *)0x70003B86 = D_00275BA8;
            while (*(short *)0x70003B86 != 0) {
                f = *p++;
                *(short *)0x70003B86 = *(short *)0x70003B86 - 1;
                if ((*f & 1) != 0 && f[3] == 3 && f[0xD] == 0) func_001A8CE0(e, f);
            }
        }
    } while (n != 0);
}

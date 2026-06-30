// NEARMISS func_00121870  (vram 0x00121870, 0xB0 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 60.48% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Hand-written-assembly memcpy: register coloring (immutable dst base in t0, running ptrs v1/a3/a1) plus a branch-likely (bnel) on the alignment guard. Structure/logic fully recovered (lq/sq quadword + ld/sd doubleword + byte tail via __int128/long long); residual is regalloc/branch-likely, not cle...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

void *func_00121870(void *dst, void *src, unsigned int n) {
    unsigned char *s = (unsigned char *)src;
    unsigned char *d = (unsigned char *)dst;

    if (n >= 0x20 && (((unsigned int)src | (unsigned int)dst) & 0xF) == 0) {
        do {
            n -= 0x20;
            ((__int128 *)d)[0] = ((__int128 *)s)[0];
            ((__int128 *)d)[1] = ((__int128 *)s)[1];
            s += 0x20;
            d += 0x20;
        } while (n >= 0x20);
        if (n >= 8) {
            do {
                n -= 8;
                *(long long *)d = *(long long *)s;
                s += 8;
                d += 8;
            } while (n >= 8);
        }
    }
    n -= 1;
    if (n != (unsigned int)-1) {
        do {
            n -= 1;
            *d++ = *s++;
        } while (n != (unsigned int)-1);
    }
    return dst;
}

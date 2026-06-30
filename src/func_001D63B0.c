// NEARMISS func_001D63B0  (vram 0x001D63B0, 0xE8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.78% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation-ORDER wall. Body + structure are byte-exact under mwcc 2.3.3 (zero insert/delete; the sq-zero 128-bit init was unlocked via a TI-mode q128 type, and hoisting the GIFtag constants into locals tightened the constant coloring). Sole residual: the cursor pointer p is colored a6 by...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

// GIF/DMA packet builder. D_00275670 is an array of per-channel ring
// builders; element [idx] holds a write cursor at +0x10. Emits a GIFtag
// packet header (count 6, EOP), the GS register-address qword, then copies
// four 4-word vectors from a2/a1/t0/a3 into the packet body, and finally
// advances the cursor by 0x70. Returns the packet body pointer (cursor+0x10).
// NEARMISS 95.78% (mwcc 2.3.3): body + structure are byte-exact (no
// insert/delete); sole residual is saved/temp register COLORING of the
// cursor pointer (target keeps it in t3; mwcc colors it a6) and the
// gp-rel base chain (v1/t5 vs a5) -- the documented mwcc regalloc-ORDER
// wall, not a logic difference.

typedef long long q128 __attribute__((mode(TI)));

extern unsigned char **D_00275670;

unsigned char *func_001D63B0(int idx, int *a1, int *a2, int *a3, int *t0) {
    unsigned char **slot;
    unsigned char *p;
    int tag = 0x50000005;
    long long mode = (long long)0x8001 | ((long long)0x4024C000 << 0x20);
    int hdr = 0x10;
    int cnt = 6;

    slot = (unsigned char **)((unsigned char *)D_00275670 + idx * 4);
    *(unsigned char *)(*(unsigned char **)((unsigned char *)slot + 0x10) + 0x3) = hdr;
    *(int *)(*(unsigned char **)((unsigned char *)slot + 0x10) + 0x4) = 0;
    *(short *)(*(unsigned char **)((unsigned char *)slot + 0x10) + 0x0) = cnt;
    p = *(unsigned char **)((unsigned char *)slot + 0x10);
    *(unsigned char **)((unsigned char *)slot + 0x10) = p + 0x70;
    *(q128 *)(p + 0x10) = 0;
    *(unsigned int *)(p + 0x1C) = tag;
    *(long long *)(p + 0x20) = mode;
    *(long long *)(p + 0x28) = 0x4141;
    *(int *)(p + 0x30) = a2[0];
    *(int *)(p + 0x34) = a2[1];
    *(int *)(p + 0x38) = a2[2];
    *(int *)(p + 0x3C) = a2[3];
    *(int *)(p + 0x40) = a1[0];
    *(int *)(p + 0x44) = a1[1];
    *(int *)(p + 0x48) = a1[2];
    *(int *)(p + 0x4C) = 0;
    *(int *)(p + 0x50) = t0[0];
    *(int *)(p + 0x54) = t0[1];
    *(int *)(p + 0x58) = t0[2];
    *(int *)(p + 0x5C) = t0[3];
    *(int *)(p + 0x60) = a3[0];
    *(int *)(p + 0x64) = a3[1];
    *(int *)(p + 0x68) = a3[2];
    *(int *)(p + 0x6C) = 0;
    return p + 0x10;
}

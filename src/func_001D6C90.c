// NEARMISS func_001D6C90  (vram 0x001D6C90, 0x13C bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.14% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Regalloc + scheduling permutation: all loads/stores/shifts and the wide bitfield packing are logically identical (96.1% with 2.3.3). Residuals are register coloring (v0/v1 swap on the first two temps; arg-packing temporaries colored to $a4..$a7 vs target $t0..$t3) and scheduling of the 0x10000000...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// NEARMISS 96.1% (mwcc 2.3.3) / 85.8% (mwcc 991202). Logic fully recovered;
// builds a 0x60-byte GS/GIF DMA packet into the per-slot buffer at
// (*(D_00275670 + arg0*4 + 0x10)) and advances that pointer by 0x60.
// Residuals are register allocation (v0/v1 swap on the first temps; the
// arg-packing temporaries land in $a4..$a7 where the target uses $t0..$t3 of
// the same EE 8-int-arg ABI but the live ranges differ) and instruction
// scheduling of the constant materializations / the 128-bit zero store
// addressing. Not the clean-store nop. Permuter (regalloc + scheduling)
// territory; 2.3.3 is already the better of the two builds.
//
// Semantics: packs the wide bitfields from the many int args into the
// GIFtag/AD register doublewords at p+0x20..p+0x58 (GS register addresses
// 0x8003/0xE/0x3B/0x47/0x42 etc.), zeroes the 0x10 quadword, writes the
// 0x1C control word 0x50000004 and the 0x00/0x03/0x04 header bytes.
typedef int u128 __attribute__((mode(TI)));
extern char *D_00275670;

char *func_001D6C90(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5,
                    int arg6, int arg7, int arg8, int arg9, int arg10,
                    int arg11, int arg12, int arg13, int arg14) {
    char *base;
    char *p;

    base = D_00275670 + (arg0 * 4);
    *(char *)(*(char **)(base + 0x10) + 3) = 0x10;
    *(int *)(*(char **)(base + 0x10) + 4) = 0;
    *(short *)(*(char **)(base + 0x10) + 0) = 5;
    p = *(char **)(base + 0x10);
    *(char **)(base + 0x10) = p + 0x60;
    *(u128 *)(p + 0x10) = 0;
    *(int *)(p + 0x1C) = 0x50000004;
    *(long long *)(p + 0x20) = 0x8003 | ((long long)0x10000000 << 32);
    *(long long *)(p + 0x28) = 0xE;
    *(long long *)(p + 0x30) = (long long)(int)arg1;
    *(long long *)(p + 0x38) = 0x3B;
    *(long long *)(p + 0x40) =
        (long long)(int)arg2 |
        ((long long)(int)arg3 << 1) |
        ((long long)(int)arg4 << 4) |
        ((long long)(int)arg5 << 12) |
        ((long long)(int)arg6 << 14) |
        ((long long)(int)arg7 << 15) |
        ((long long)arg8 << 16) |
        ((long long)arg9 << 17);
    *(long long *)(p + 0x48) = 0x47;
    *(long long *)(p + 0x50) =
        (long long)arg10 |
        ((long long)arg11 << 2) |
        ((long long)arg12 << 4) |
        ((long long)arg13 << 6) |
        ((long long)arg14 << 32);
    *(long long *)(p + 0x58) = 0x42;
    return p + 0x10;
}

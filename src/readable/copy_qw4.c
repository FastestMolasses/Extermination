// NEARMISS copy_qw4  (vram 0x00102958, 0x24 bytes) — readable companion C, NOT byte-identical.
//
// objdiff 94.67% via ee-gcc 2.9-991111-01 (-O2) when compiled on its own (mwcc 2.3 / 2.3.3 / 2.4 give
// 96.22%; this is SDK code, so ee-gcc is the compiler of record). Object
// similarity does not prove semantic equivalence. Remaining differences:
// Hand-written: the original uses a2..t1 for the four quadwords; compilers pick other registers.
//
// The function links from the asm body in src/copy_qw4.c (kept by the user's asm-bodies decision
// and so that matched_code does not drop); this companion is the readable ground truth and is
// never compiled by tools/decomp/build.py (src/readable/ is not scanned). Registry:
// docs/NEARMISS.md. First-level lane 2026-10-01 (docs/FIRST_LEVEL_DECOMP.md).
//
// COMPILER: eegcc
// CFLAGS: -O2

// SDK: copies a 4x4 matrix as four 128-bit quadwords (all four loaded
// before the stores, so dst may alias src).
typedef unsigned int u128 __attribute__((mode(TI)));

void copy_qw4(u128 *dst, u128 *src) {
    u128 r0 = src[0];
    u128 r1 = src[1];
    u128 r2 = src[2];
    u128 r3 = src[3];

    dst[0] = r0;
    dst[1] = r1;
    dst[2] = r2;
    dst[3] = r3;
}

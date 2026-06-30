// NEARMISS func_001D4F30  (vram 0x001D4F30, 0x80 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 82.50% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 4). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation + scheduling permutation: loop structure, the min(t1,0x1f8) clamp, and all stores are logically correct, but mwcc colors the loop-body temporaries differently across the whole body and schedules the t2=a0<<2 hoist and the t1-=0x1f8 decrement at different points than the target...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4

// NEARMISS 82.5% (mwcc233). Chunked command-list writer: splits the count
// *a1 into runs of at most 0x1f8 and stages one 0x10-byte record per run into
// the per-slot list at D_00275670[a0<<2]. Each record: tag byte 0x30, the
// running source pointer (a1+0x40, advanced by n*0x820 each chunk), and a
// halfword n*0x82; the slot write pointer at +0x10 advances by 0x10 per record.
// Loop exits when the remaining count goes non-positive. Loop structure, the
// min(t1,0x1f8) clamp, and all stores are logically correct; the residual is
// register coloring across the loop body plus scheduling of the a0<<2 hoist and
// the -0x1f8 decrement. Permuter (regalloc/scheduling).
extern char *D_00275670;

void func_001D4F30(int a0, int *a1)
{
    int t1;
    char *t0;
    int t2;
    int n;
    int v1;
    int hw;
    char *a3;
    char *ptr;

    t1 = *a1;
    t0 = (char *)(a1 + 0x10);
    t2 = a0 << 2;
    if (t1 > 0) {
        do {
            n = 0x1f8;
            if (t1 < 0x1f8) {
                n = t1;
            }
            a3 = D_00275670 + t2;
            v1 = (n << 6) + n;
            hw = v1 << 1;
            v1 = v1 << 5;
            ptr = *((char **)(a3 + 0x10));
            t1 -= 0x1f8;
            ptr[3] = 0x30;
            ptr = *((char **)(a3 + 0x10));
            *((char **)(ptr + 4)) = t0;
            t0 += v1;
            ptr = *((char **)(a3 + 0x10));
            *((short *)ptr) = (short)hw;
            ptr = *((char **)(a3 + 0x10));
            *((char **)(a3 + 0x10)) = ptr + 0x10;
        } while (t1 > 0);
    }
}

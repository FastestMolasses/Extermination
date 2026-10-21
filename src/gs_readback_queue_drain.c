// NEARMISS gs_readback_queue_drain  (vram 0x001D7410, 0x100 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.77% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// Corrected callee (dma_wait_and_submit for the stale alias sub_D2_TADR_08x); locals reordered. Left: the frame index is loaded into $a2 where mwcc picks $a0.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// CORRECTED (decomp lane round 5, 2026-10-20): the transfer kick calls
// dma_wait_and_submit(0, 0), the 0x00100A60 entry the original calls (the old C
// used its stale pre-rename alias sub_D2_TADR_08x). Locals reordered so mwcc 2.3.3
// gives the loop counter / entry / offset the target's saved registers.

//
// Semantics: if global count D_00275C08 != 0, iterate i over [0,count). Each
// iteration indexes a 0x48-stride record e within a per-actor row of the
// D_008172C0 table (row = (D_00275670+0x9C)*0x480). Reads two floats e[0],e[1],
// passes them through float_to_int, then builds a DMA/transfer descriptor in a
// 0x70-byte stack scratch via gs_readback_build_packet (args: scratch, sign-extended
// (D_00810F10 & 0x1FF)<<5, D_00810F02 & 0x3F, 0x31, the two float results, 8, 8),
// flushes the data cache (FlushCache(0)), copies e+8 into the scratch (gs_vram_readback), and kicks
// the transfer (dma_wait_and_submit). After the loop, clears the count D_00275C08 = 0.
// Note D_00810F10/D_00810F02 are absolute-addressed (declared as arrays to push
// them past the sdata threshold so they are NOT gp-relative, matching the target).

extern int *D_00275670;
extern int D_00275C08;
extern unsigned char D_00810F02[];
extern unsigned short D_00810F10[];
extern char D_008172C0[];

extern int float_to_int(float f);
extern void gs_readback_build_packet(void *p, long a1, int a2, int a3, int t0, int t1, int t2, int t3);
extern void FlushCache(int ch);
extern void gs_vram_readback(void *p, char *src);
extern int dma_wait_and_submit(int a, int b);

void gs_readback_queue_drain(void) {
    int r1;
    int r0;
    int i;
    char *e;
    int off;
    char sp50[0x70];

    if (D_00275C08 != 0) {
        i = 0;
        off = 0;
        while (i < D_00275C08) {
            e = off + (D_008172C0 + *(int *)((char *)D_00275670 + 0x9C) * 0x480);
            r0 = float_to_int(*(float *)(e + 0));
            r1 = float_to_int(*(float *)(e + 4));
            gs_readback_build_packet(sp50, (short)((D_00810F10[0] & 0x1FF) << 5), D_00810F02[0] & 0x3F, 0x31, r0, r1, 8, 8);
            FlushCache(0);
            gs_vram_readback(sp50, e + 8);
            dma_wait_and_submit(0, 0);
            off += 0x48;
            i += 1;
        }
    }
    D_00275C08 = 0;
}

// NEARMISS gs_readback_queue_drain  (vram 0x001D7410, 0x100 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.14% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 8). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring permutation of the two loop vars: target e->s1/off->s2 with field temp in a2; mwcc gives e->s2/off->s1 with temp in a0. Frame (0xC0), instruction selection, gp-rel/absolute addressing and scheduling all match. Permuter-class.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 8

//
// NEARMISS 99.14% (mwcc 2.3.3; pinned 991202 reaches 89.9%). Body, structure,
// instruction selection, frame size (0xC0), gp-rel vs absolute global addressing,
// and scheduling are all verified identical to the target. The SOLE residual is a
// register-coloring permutation between the two loop induction vars: the target
// colors e->$s1 and off->$s2 (and uses $a2 as the field-index temp), whereas mwcc
// colors e->$s2/off->$s1 and uses $a0 for that temp. Permuter territory.
//
// Semantics: if global count D_00275C08 != 0, iterate i over [0,count). Each
// iteration indexes a 0x48-stride record e within a per-actor row of the
// D_008172C0 table (row = (D_00275670+0x9C)*0x480). Reads two floats e[0],e[1],
// passes them through func_001281C0, then builds a DMA/transfer descriptor in a
// 0x70-byte stack scratch via func_00100D78 (args: scratch, sign-extended
// (D_00810F10 & 0x1FF)<<5, D_00810F02 & 0x3F, 0x31, the two float results, 8, 8),
// disables a DMAC handler, copies e+8 into the scratch (func_00100EB8), and kicks
// the transfer (sub_D2_TADR_08x). After the loop, clears the count D_00275C08 = 0.
// Note D_00810F10/D_00810F02 are absolute-addressed (declared as arrays to push
// them past the sdata threshold so they are NOT gp-relative, matching the target).

extern int *D_00275670;
extern int D_00275C08;
extern unsigned char D_00810F02[];
extern unsigned short D_00810F10[];
extern char D_008172C0[];

extern int func_001281C0(float f);
extern void func_00100D78(void *p, long a1, int a2, int a3, int t0, int t1, int t2, int t3);
extern void DisableDmacHandler(int ch);
extern void func_00100EB8(void *p, char *src);
extern void sub_D2_TADR_08x(int a, int b);

void gs_readback_queue_drain(void) {
    char sp50[0x70];
    int i;
    int off;
    char *e;
    int r0;
    int r1;

    if (D_00275C08 != 0) {
        i = 0;
        off = 0;
        while (i < D_00275C08) {
            e = off + (D_008172C0 + *(int *)((char *)D_00275670 + 0x9C) * 0x480);
            r0 = func_001281C0(*(float *)(e + 0));
            r1 = func_001281C0(*(float *)(e + 4));
            func_00100D78(sp50, (short)((D_00810F10[0] & 0x1FF) << 5), D_00810F02[0] & 0x3F, 0x31, r0, r1, 8, 8);
            DisableDmacHandler(0);
            func_00100EB8(sp50, e + 8);
            sub_D2_TADR_08x(0, 0);
            off += 0x48;
            i += 1;
        }
    }
    D_00275C08 = 0;
}

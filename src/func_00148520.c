// NEARMISS func_00148520  (vram 0x00148520, 0x1B8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.27% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-coloring + instruction-scheduling permutation at 99.27% (mwcc 2.3.3). Body fully correct. Two residual clusters: (a) switch-byte colored $a0 vs target $a1 (keeps arg0 in $s1); (b) the two mtc1 arg moves of the 2nd anim_clip_init are emitted f12-then-f13 by mwcc but f13-then-f12 in target...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

// NEARMISS 99.27% (mwcc 2.3.3; mwcc 991202 = 92.64%). Body/structure fully
// recovered: a 3-state machine on *(u8*)(self+6) driving anim_clip_init /
// func_001EFD90 / func_001FBD50 setup. Sole residuals are pure compiler
// artifacts: (1) register-coloring of the switch byte (target reads it into
// $a1 keeping arg0/$a0 -> $s1; mwcc colors it into $a0), and (2) the two
// independent mtc1 arg-moves for the 2nd anim_clip_init(arg0,8,1.0f,0.0f) are
// emitted in the target as mtc1 zero,f13 BEFORE mtc1 v0,f12 while mwcc emits
// f12 then f13 (the (z=0.0f) fp-arg-order idiom does NOT flip 2.3.3 here).
// Both are regalloc/scheduling permutations -> permuter territory.
extern int anim_clip_init(char *self, int clip, float a, float b);
extern void func_00102948(void *dst, void *src);
extern void func_001E8B90(void *p, float f);
extern void func_001EFD90(int mode, void *b, void *c);
extern void func_001FBD50(char *p, int a, int b, float f);
extern void func_0021C040(char *a, char *b);
extern unsigned char D_008102B0;
extern unsigned char D_008106BD;
extern unsigned char D_700038A0;

void func_00148520(char *arg0, char *arg1) {
    int v;
    unsigned char st;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(int *)(arg1 + 0x40) = 0x3F800000;
        *(char *)(arg1 + 0x88) = 0;
        *(char *)(arg1 + 0x83) = 0;
        *(int *)(arg1 + 0x50) = 0;
        v = *(char *)(arg1 + 0x8D) & 1;
        *(char *)(arg1 + 0x8D) = v;
        anim_clip_init((char *)v, 7, 1.0f, 0.0f);
        if (*(char *)(arg1 + 0x86) != 0) {
            *(char *)(arg1 + 0x86) = 0;
            func_0021C040((char *)&D_008102B0, arg0);
            if (D_008106BD == 1) {
                D_008106BD = 0;
            }
        }
        /* fallthrough */
    case 1:
        if (*(char *)(arg1 + 0x8D) == 1) {
            *(char *)(arg1 + 0x8D) = *(char *)(arg1 + 0x8D) | 2;
            func_00102948(&D_700038A0, arg0 + 0xB0);
            *(float *)0x700038A4 = *(float *)(arg1 + 0x6C);
            func_001EFD90(0x80000016, &D_700038A0, arg0 + 0xC0);
            func_001E8B90(&D_700038A0, 5.0f);
        }
        if (*(float *)(arg1 + 0x48) <= 0.0f && (*(char *)(arg1 + 0x81) & 2)) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(int *)(arg1 + 0x4C) = 0;
            *(float *)(arg1 + 0x48) = 0.0f;
            func_001FBD50(arg0, 0x7E6, 0, 300.0f);
            anim_clip_init(arg0, 8, 1.0f, 0.0f);
        }
        break;
    case 2:
        if (*(int *)(arg1 + 0x20) & 0x1000) {
            *(char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
        }
        break;
    }
}

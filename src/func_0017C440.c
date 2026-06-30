// NEARMISS func_0017C440  (vram 0x0017C440, 0xFC bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 96.65% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// CW-vs-mwcc branch-delay-slot scheduling: target leaves the `bne v1,v0` (idx==2 selector) delay slot as nop; both mwcc991 and mwcc233 speculatively hoist the pure `lui at,0x7000` (address-high of the 0x70003A20 global, recomputed per arm) into the slot and duplicate it at the arm head. Body 100% r...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// NEARMISS (96.65% mwcc233 / 88.70% mwcc991, body byte-identical):
// Clip-rewind handler. Decrements the per-entity clip index at p+0x25C
// (from the base count at p+0x23F), stores the matching speed scalar from
// table D_00248870[idx] into p+0x38, runs func_00178B90 for side effects,
// then resolves a clip id via func_0017B490(p,1,kind=p+0x235,idx) (low 16
// bits, sign-extended) and pushes a frame value via func_001C61D0 into the
// global FP scratch at 0x70003A20. Depending on whether idx==2 it arbitrates
// the new clip with a -18.0 or -46.0 phase offset (4.0 rate), then flags
// p+0x1F0 = 1.
//
// SOLE RESIDUAL: the `bne v1,v0` that selects the idx==2 arm has a `nop`
// delay slot in the CW target; mwcc (both 991 and 233) speculatively hoists
// the pure `lui at,0x7000` (address-high of the 0x70003A20 global, recomputed
// per arm) into that branch slot and re-emits a duplicate at the arm head.
// This is a CW-vs-mwcc branch-delay-slot scheduling difference, NOT the
// clean-store delay-slot-nop class that mwcc 2.3.3 unlocks, so 2.3.3 does not
// close it. Body/structure/control-flow are fully recovered and verified
// byte-identical everywhere except those branch-slot instructions.
extern float D_00248870[];
extern void func_00178B90(void);
extern int func_0017B490(char *p, int a, unsigned char b, unsigned char c);
extern int func_001C61D0(int a, int b);
extern int anim_clip_arbiter(char *p, int v, float a, float b);

void func_0017C440(char *p) {
    int v;

    *(unsigned char *)(p + 0x25C) = *(unsigned char *)(p + 0x23F) - 1;
    *(float *)(p + 0x38) = D_00248870[*(unsigned char *)(p + 0x25C)];
    func_00178B90();
    v = (short)func_0017B490(p, 1, *(unsigned char *)(p + 0x235), *(unsigned char *)(p + 0x25C));
    *(float *)0x70003A20 = (float)func_001C61D0(*(int *)(p + 0x40), v);
    if (*(unsigned char *)(p + 0x25C) == 2) {
        anim_clip_arbiter(p, v, 4.0f, *(float *)0x70003A20 - 18.0f);
    } else {
        anim_clip_arbiter(p, v, 4.0f, *(float *)0x70003A20 - 46.0f);
    }
    *(char *)(p + 0x1F0) = 1;
}

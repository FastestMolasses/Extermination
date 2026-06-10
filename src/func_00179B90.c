// Footstep sound-id variant roll: rand 0..4 (non-uniform fold of
// rand&7 — 5..7 wrap to 0..2). Used by the footstep mapper
// func_00182430 (id = block base + gait sub-base + this) and the
// gear layer (0x138 + this). See FINDINGS "FOOTSTEP SURFACE TABLE".
//
// Matching notes (idiom 16b refinement — partial crack of the
// slt-into-branch wall's fresh-temp case): a plain `ok = r < 5;
// if (!ok)` still $at-folds (99.17%). The match needs the compare
// assigned back into the COMPARED variable after copying its value
// out: `r = t; t = t < 5; if (!t)`. The kill-rename splits t's web
// (def 1 = the andi, def 2 = the compare) and mwcc allocates the
// compare web the next free register — CW's `slti v1, v0, 5; bnez
// v1` exactly. The bnez delay-slot nop survives because the only
// fall-through candidate (`r -= 5`) clobbers the live return value
// (wall-#13 safe case).
extern int func_00122BB8(void);

int func_00179B90(void) {
    int t = func_00122BB8() & 7;
    int r = t;
    t = t < 5;
    if (!t) {
        r -= 5;
    }
    return r;
}

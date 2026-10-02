// CFLAGS: -O4,p -sdatathreshold 0
// Plays the halfword sound id at D_0024DB80 + (self+0x56 high byte) * 4 + which * 2 (rows of
// two halfwords) through func_001FBD50(self, sound, 0, 300.0).
// Corrected 2026-10-01 against the original instructions (docs/FINDINGS.md "NEARMISS body
// corrections from the level side-track lanes"): the third argument is 0 (the earlier text
// passed the caller's a2 through). objdiff 100% (mwcc 991202).
extern short D_0024DB80[];
extern void func_001FBD50(void *self, int sound, int a2, float radius);

void func_001BBD20(char *self, int which) {
    float r = 300.0f;
    int row = (int)D_0024DB80 + (((*(short *)(self + 0x56) & 0xff00) >> 8) << 2);
    func_001FBD50(self, *(unsigned short *)((which << 1) + row), 0, r);
}

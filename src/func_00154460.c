// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// SEMANTICS (resolved s33): tendril-field trigger gate. Returns 1 iff
// gameplay frame (spad 0x70003B8D == 0) AND the player (D_008102B0
// +0xA0/A4/A8) is within 3*recX / 3+recY / 3*recZ of the actor origin
// +0xB0, rec = D_00248120 + self[0xD]*20 (PARENT pad kind footprint).
// The three footprint components are three separate pooled symbols
// (D_00248120 / D_00248128 / D_00248124), each indexed at offset 0 by
// self[0xD]*20 -- NOT a single base with +8/+4 displacements.
// The final `if(cond) goto inside; return 0; inside: return 1;` is what
// reproduces CW's branch-on-true + duplicated dead `li v0,1` tail; the
// pinned 991202 build coalesces that tail and tops out at 83.7%.
extern float D_008102B0[];
extern unsigned char D_00248120[];
extern unsigned char D_00248124[];
extern unsigned char D_00248128[];
extern float func_0011DF78(float);

int func_00154460(unsigned char *self) {
    float *plr = D_008102B0;
    float d;
    if (*(volatile unsigned char *)0x70003B8D != 0) return 0;
    d = func_0011DF78(plr[0x28] - *(float *)(self + 0xB0));
    if (!(d <= 3.0f * *(float *)(D_00248120 + self[0xD] * 20))) return 0;
    d = func_0011DF78(plr[0x2A] - *(float *)(self + 0xB8));
    if (!(d <= 3.0f * *(float *)(D_00248128 + self[0xD] * 20))) return 0;
    d = func_0011DF78(plr[0x29] - *(float *)(self + 0xB4));
    if (d <= 3.0f + *(float *)(D_00248124 + self[0xD] * 20)) goto inside;
    return 0;
inside:
    return 1;
}

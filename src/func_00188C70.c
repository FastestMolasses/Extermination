// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// Per-frame slice for the global actor record D_008102B0. If the engine flag
// D_008106CC is set, just stamps the caller's state byte (p+4) = 3 and returns.
// Otherwise, when the actor is in state 1 (D_008102B0+4 == 1), not flagged at
// +0x275 (==4), has action byte +0x1F0 == 0x33, its anim id +0x20C matches one
// of two clip ids (D_00248BA0/D_00248C80), and its clip time +0x3C <= 50.0:
//   - clip time >= 39.0: if exactly 39.0 call func_001F4010(9, *D_00275B40+0x90)
//     then return (>39 just returns).
//   - clip time > 27.0: clear p+1 and fall through to the default publish.
//   - clip time in [8.0, 27.0]: copy_qw4 from the +0x15C target pointer (+0x90)
//     into the publish slot (*D_00275B40)+0x90 and return.
//   - clip time < 8.0: fall through to the default publish.
// Default publish: copy_qw4 from the +0x120 pointer (loaded here via the
// separate sym D_008103D0 == D_008102B0+0x120) into *D_00275B40+0x90, then
// func_001026A0(*D_00275B40+0xC0, *D_00275B40+0x90, D_0024A340).
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906); the pinned 991202 build only
// reaches 97.85% (loop/branch scheduling). Following sibling func_00188B80, the
// far globals are oversized so only the gp-rel pointer D_00275B40 lands in sdata
// at threshold 4. objdiff 100.0% vs expected (the fv0f/fv1 shown in the diff are
// display aliases for the same physical register $f1; all diff_kinds empty).
extern void copy_qw4(char *dst, char *src);
extern void func_001026A0(char *a, char *b, void *c);
extern void func_001F4010(int a, char *b);
extern short D_00248BA0[16];
extern short D_00248C80[16];
extern char D_0024A340[16];
extern char **D_00275B40;
extern char D_008102B0[1024];
extern char *D_008103D0[16];
extern unsigned char D_008106CC[16];

void func_00188C70(unsigned char *p) {
    char *g;
    float f;

    g = D_008102B0;
    if (D_008106CC[0] != 0) {
        *(signed char *)(p + 4) = 3;
        return;
    }
    if (*(unsigned char *)(g + 4) == 1 &&
        *(unsigned char *)(g + 0x275) == 4 &&
        *(unsigned char *)(g + 0x1F0) == 0x33 &&
        (*(short *)(g + 0x20C) == D_00248BA0[0] || *(short *)(g + 0x20C) == D_00248C80[0]) &&
        (f = *(float *)(g + 0x3C)) <= 50.0f) {
        if (!(f < 39.0f)) {
            if (f == 39.0f) {
                func_001F4010(9, *D_00275B40 + 0x90);
            }
            return;
        }
        if (!(f <= 27.0f)) {
            *(signed char *)(p + 1) = 0;
        } else if (!(f < 8.0f)) {
            copy_qw4(*D_00275B40 + 0x90, *(char **)(g + 0x15C) + 0x90);
            return;
        }
    }
    copy_qw4(*D_00275B40 + 0x90, D_008103D0[0] + 0x90);
    func_001026A0(*D_00275B40 + 0xC0, *D_00275B40 + 0x90, D_0024A340);
}

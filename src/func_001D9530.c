// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
//
// HUD/overlay setup for actor arg0. Copies three 16-byte layout descriptor
// quadwords (D_00253190/A0/B0) onto the stack, resolves three glyph/sprite
// handles via func_001C6120(font D_0028A56C, code 0x10/0x11/0x16), then picks a
// horizontal extent f20: 650.0 when func_001B0070() bit 0x80 is set or when the
// level selector (D_00810700<<8 | D_00810701) is one of {0x201,0x300,0x1000,
// 0x1200}, else 120.0. Runs func_001DA290(3,0), then lays out the three pieces
// with func_001D91A0(actor, slot, handle, flag, f20). Finally writes through
// the global cursor *(D_00275670+0x1C): byte +3 = 0x60, word +4 = 0, half +0 =
// 0, advances the cursor by 0x10, and flushes via func_001CB760.
//
// Built with mwcc 2.3.3 (mwccps2-2.3.3-000906), not the pinned 991202.
// -sdatathreshold 4: the 4-byte pointer D_00275670 lives in gp range and is
// gp-relative; the far globals D_0028A56C / D_00810700 / D_00810701 are sized
// as arrays (>threshold) so mwcc emits hi/lo for them, matching CW. The switch
// cases are written in reverse (mwcc reverses them back to the CW 0x1200-first
// dispatch order). Verified objdiff 100% vs build/expected/func_001D9530.o
// (991202 = 68.6%).
typedef struct { int w[4]; } Qw __attribute__((aligned(16)));

extern char *func_001C6120(int a0, int a1);
extern int func_001B0070(void);
extern void func_001DA290(int a0, int a1);
extern void func_001D91A0(int a0, void *a1, char *a2, int a3, float f);
extern void func_001CB760(void *a0, int a1, char *a2, char *a3);
extern Qw D_00253190;
extern Qw D_002531A0;
extern Qw D_002531B0;
extern char *D_00275670;
extern int D_0028A56C[2];
extern char D_007635C0[64];
extern unsigned char D_00810700[8];
extern unsigned char D_00810701[8];

void func_001D9530(int arg0) {
    Qw sp70;
    Qw sp80;
    Qw sp90;
    char *s0;
    char *s1;
    char *s2;
    char *s3;
    char *g;
    float f20;
    int mode;

    sp70 = D_00253190;
    sp80 = D_002531A0;
    sp90 = D_002531B0;
    s0 = func_001C6120(D_0028A56C[0], 0x10);
    s1 = func_001C6120(D_0028A56C[0], 0x11);
    s2 = func_001C6120(D_0028A56C[0], 0x16);
    s3 = *(char **)(D_00275670 + 0x1C);
    if (func_001B0070() & 0x80) {
        f20 = 650.0f;
    } else {
        mode = (D_00810700[0] << 8) + D_00810701[0];
        switch (mode) {
        case 0x1000:
        case 0x300:
        case 0x201:
        case 0x1200:
            f20 = 650.0f;
            break;
        default:
            f20 = 120.0f;
            break;
        }
    }
    func_001DA290(3, 0);
    func_001D91A0(arg0, &sp80, s0, 0, f20);
    func_001D91A0(arg0, &sp70, s1, 0, f20);
    func_001D91A0(arg0, &sp90, s2, 1, f20);
    g = D_00275670;
    *(char *)(*(char **)(g + 0x1C) + 3) = 0x60;
    *(int *)(*(char **)(g + 0x1C) + 4) = 0;
    *(short *)(*(char **)(g + 0x1C) + 0) = 0;
    *(char **)(g + 0x1C) = *(char **)(g + 0x1C) + 0x10;
    func_001CB760(D_007635C0, 0, s3, g);
}

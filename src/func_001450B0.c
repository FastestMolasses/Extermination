// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Byte-matched (objdiff 100%), matching wave 2026-10-28 (was an asm-word body).
// Per-frame update for one enemy class, (e, d) with d the behaviour record.
// Runs the behaviour handler for e[5] (0 func_00145210, 1 func_001453C0,
// 2 func_001455A0; other values none). While the current animation (low byte
// of d+0x30) is 9 or 8 it fires one of five random voice cues (0x827.. or
// 0x82C.., chosen by ((rand >> 13) % 5)) through func_001FBD50 at range
// 300.0. Then the fixed chain: func_00146CE0, func_00145880, func_001459A0,
// d+0x30 = anim_advance_time(e, 1.0f), func_00131ED0, func_001C68C0,
// func_001B17A0 and the unconditional hook at e+0x4C. Same cue block as the
// byte-matched func_00142070.
extern void func_00145210(unsigned char *, unsigned char *);
extern void func_001453C0(unsigned char *, unsigned char *);
extern void func_001455A0(unsigned char *, unsigned char *);
extern int func_00122BB8(void);
extern void func_001FBD50(unsigned char *, int, int, float);
extern void func_00146CE0(unsigned char *, unsigned char *);
extern void func_00145880(unsigned char *, unsigned char *);
extern void func_001459A0(unsigned char *, unsigned char *);
extern int anim_advance_time(unsigned char *, float);
extern void func_00131ED0(unsigned char *);
extern void func_001C68C0(unsigned char *);
extern void func_001B17A0(unsigned char *);

void func_001450B0(unsigned char *e, unsigned char *d) {
    switch (e[5]) {
    case 0:
        func_00145210(e, d);
        break;
    case 1:
        func_001453C0(e, d);
        break;
    case 2:
        func_001455A0(e, d);
        break;
    }
    if (d[0x30] == 9) {
        func_001FBD50(e, ((func_00122BB8() >> 13) % 5) + 0x827, 0, 300.0f);
    } else if (d[0x30] == 8) {
        func_001FBD50(e, ((func_00122BB8() >> 13) % 5) + 0x82C, 0, 300.0f);
    }
    func_00146CE0(e, d);
    func_00145880(e, d);
    func_001459A0(e, d);
    *(int *)(d + 0x30) = anim_advance_time(e, 1.0f);
    func_00131ED0(e);
    func_001C68C0(e);
    func_001B17A0(e);
    (*(void (**)(unsigned char *))(e + 0x4C))(e);
}

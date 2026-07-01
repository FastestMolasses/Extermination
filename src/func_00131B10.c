// NEARMISS func_00131B10  (vram 0x00131B10, 0x368 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 95.14% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// mwcc-vs-CW branch-lowering / dead-trailing-block wall: target's beql-dispatched case-0 leaves a duplicated dead addiu v0,a3,1 after the not-taken exit branch (CW's lowering keeps a redundant recompute that mwcc's dead-code pass elides), plus one anim_clip_init(arg0,5,5.0f,0.0f) call site emits it...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Per-state animation/AI driver dispatched on the state byte at arg0+6 (states
// 0-3). State 0: advances state, plays a "ready" sound cue via func_001B1190 and
// a text/HUD event func_001FBD50(0x7D8), then either (owner flag arg0+0xD bit1
// set) jumps straight to state 3 and kicks clip 4 or 5 depending on arg1+0x61,
// or otherwise picks clip 0x29/0x2A/0x2B (0x2A gated by func_00122BB8 bit 9,
// a difficulty/RNG-style check) when arg1+0x61 is clear. State 1: plays a
// transition sound when the current clip (arg0+0x2C masked) and pose angle
// (arg0+0x3C) match specific clip/angle pairs, then on the owner's 0x1000 input
// bit advances state and (unless owner flag bit1 set) fires an unbind event
// func_001FBD50(0x7D7) and calls func_001EFE00(0x8000001E) to release a handler,
// marking arg0+4=3 if that fails. State 2: on owner flag bit1 + the two D_0028A9A0/
// D_0028A9A2 gate globals, marks arg0+4=3 and propagates that to a linked actor at
// arg0+0x24 (clearing the link). State 3: waits for D_008107EA==2, then reverts to
// state 1, resets arg1+0x34, rebinds via func_001EFE00(0x80000046), and kicks clip
// 0x2A/0x2B (same func_00122BB8 bit-9 gate as state 0).
extern void anim_clip_init(char *self, int clip, float a, float b);
extern int func_00122BB8(void);
extern void func_001B1190(int a0);
extern char *func_001EFE00(int a0, char *a1);
extern void func_001FBD50(char *self, int a, int b, float f);
extern short D_0028A9A0;
extern char D_0028A9A2;
extern unsigned char D_008107EA;

void func_00131B10(char *arg0, char *arg1) {
    unsigned char st;
    int a1v;
    char *n;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        func_001B1190(*(unsigned char *)(arg0 + 0x9A));
        func_001FBD50(arg0, 0x7D8, 0, 300.0f);
        if (*(unsigned char *)(arg0 + 0xD) & 2) {
            *(unsigned char *)(arg0 + 6) = 3;
            *(int *)(arg1 + 0x34) = 0x3F800000;
            D_008107EA = 1;
            if (*(unsigned char *)(arg1 + 0x61) != 0) {
                anim_clip_init(arg0, 4, 5.0f, 0.0f);
                return;
            }
            anim_clip_init(arg0, 5, 5.0f, 0.0f);
            return;
        }
        *(int *)(arg1 + 0x34) = 0x3F800000;
        if (*(unsigned char *)(arg1 + 0x61) != 0) {
            anim_clip_init(arg0, 0x29, 5.0f, 0.0f);
            return;
        }
        if ((func_00122BB8() >> 9) & 1) {
            anim_clip_init(arg0, 0x2A, 5.0f, 0.0f);
            return;
        }
        anim_clip_init(arg0, 0x2B, 5.0f, 0.0f);
        return;
    case 1:
        a1v = *(short *)(arg0 + 0x2C) & 0xFFFF7FFF;
        if (a1v == 0x2A) {
            if (*(float *)(arg0 + 0x3C) == 34.0f) {
                func_001FBD50(arg0, 0x7D5, 0, 300.0f);
            } else {
                goto check21;
            }
        } else {
check21:
            if (a1v == 0x2B && *(float *)(arg0 + 0x3C) == 21.0f) {
                func_001FBD50(arg0, 0x7D5, 0, 300.0f);
            }
        }
        if (*(unsigned short *)(arg1 + 0x58) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            if (!(*(unsigned char *)(arg0 + 0xD) & 2)) {
                *(char *)(arg1 + 0x6D) = 0;
                func_001FBD50(arg0, 0x7D7, 0, 300.0f);
                if (func_001EFE00(0x8000001E, arg0) == 0) {
                    *(char *)(arg0 + 4) = 3;
                    return;
                }
            }
        }
        return;
    case 2:
        if ((*(unsigned char *)(arg0 + 0xD) & 2) && D_0028A9A2 != 0 && D_0028A9A0 == 2) {
            *(char *)(arg0 + 4) = 3;
            n = *(char **)(arg0 + 0x24);
            if (n != 0) {
                *(char *)(n + 4) = 3;
                *(char **)(arg0 + 0x24) = 0;
                return;
            }
        }
        break;
    case 3:
        if (D_008107EA == 2) {
            *(unsigned char *)(arg0 + 6) = 1;
            *(int *)(arg1 + 0x34) = 0x3EB33333;
            *(char **)(arg0 + 0x24) = func_001EFE00(0x80000046, arg0);
            if ((func_00122BB8() >> 9) & 1) {
                anim_clip_init(arg0, 0x2A, 5.0f, 0.0f);
                return;
            }
            anim_clip_init(arg0, 0x2B, 5.0f, 0.0f);
        }
        break;
    }
}

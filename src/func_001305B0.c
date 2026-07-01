// NEARMISS func_001305B0  (vram 0x001305B0, 0x4F8 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 99.19% via mwcc 2.3.3 (mwcps2-2.3.3-000906) (-O4,p -sdatathreshold 0). The LOGIC and STRUCTURE are faithful; the residual
// diff is a genuine compiler artifact that no source change fixes here:
// Register-allocation/scheduling permutation on 4 stack-temp float reloads (target always reloads 0x38a0 first regardless of C order; both mwcc builds reload in a fixed internal order) PLUS an idiom-13-class delay-slot-fill gap: target speculates 'lui at,0x7000' / 'move a0,arg0' into two branch del...
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0

//
// Per-state driver dispatched on the state byte at arg0+6 (switch 0..4).
// arg0 = self, arg1 = owner. State 0 inits the owner timers/flags and kicks
// clip 0x1E, then falls through to state 1, which drives the lean angle at
// self+0xC4 via func_001B1240/func_001B12B0 until the 0x1000 input bit advances
// it (clip 0x1F). State 2 waits on the same bit (clip 0x20). State 3 either
// commits the dash (func_0021BE40==0 && func_001A7B80!=0 -> seed D_00810320..,
// pick the D_008104D4 speed from the 0xD flags/D_0081070A, clip 0x21) or runs
// the steering fallback (func_001B3250/func_001B1560). State 4 resets on input.
// Tail: when state>=2 and owner bit 0x69&1 is set, ticks the 0x50 countdown and
// on expiry reseeds it and fires func_001EFD90(0x8000001D, ...). Tail-calls
// func_00132490(self, owner).
extern void anim_clip_init(char *self, int clip, float a, float b);
extern void func_001028B8(void *a, void *b, void *c);
extern void func_00131F20(char *p, void *a, void *b);
extern void func_00132490(char *self, char *owner);
extern int func_00122BB8(char *p);
extern int func_001A7B80(char *p);
extern float func_001B1240(char *p, float a, float b);
extern float func_001B12B0(float a, float b, float c);
extern int func_001B1560(char *p, float *a, float b);
extern void func_001B2B10(char *p, void *a, void *b);
extern int func_001B3250(char *p, void *a, float b);
extern void func_001B55E0(char *p, int a);
extern void func_001EFD90(int a, void *b, char *c);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int func_0021BE40(unsigned char *a, char *b, int c);
extern unsigned char D_008102B0;
extern char D_008102BF;
extern float D_00810320;
extern float D_00810324;
extern float D_00810328;
extern float D_0081032C;
extern float D_00810360;
extern float D_00810368;
extern int D_008104D4;
extern unsigned char D_0081070A;
extern char D_700038A0[];

void func_001305B0(char *arg0, char *arg1) {
    unsigned char st;
    unsigned char fl;
    unsigned short c;
    float f0, f1, f2, f3;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(char *)(arg1 + 0x63) = 0;
        *(short *)(arg1 + 0x50) = 0;
        *(short *)(arg1 + 0x54) = 1;
        *(char *)(arg1 + 0x6B) = 1;
        *(int *)(arg1 + 0x3C) = 0;
        *(float *)(arg1 + 0x34) = 2.0f;
        *(char *)(arg0 + 0x5C) = 2;
        anim_clip_init(arg0, 0x1E, 5.0f, 0.0f);
        /* fallthrough */
    case 1:
        if (*(unsigned short *)(arg1 + 0x54) != 0 && *(float *)(arg0 + 0x3C) <= 136.0f) {
            *(short *)(arg1 + 0x54) = 0;
            func_001FBD50(arg0, 0x7E1, 0, 300.0f);
        }
        if (*(unsigned short *)(arg1 + 0x58) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            *(float *)(arg1 + 0x34) = 1.0f;
            anim_clip_init(arg0, 0x1F, 1.0f, 0.0f);
        } else {
            *(float *)(arg1 + 0x30) = func_001B1240(arg0 + 0xB0, D_00810360, D_00810368);
            *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x30), *(float *)(arg0 + 0xC4), 0.0174532925f);
        }
        break;
    case 2:
        if (*(unsigned short *)(arg1 + 0x58) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            anim_clip_init(arg0, 0x20, 1.0f, 0.0f);
        }
        break;
    case 3:
        if (func_0021BE40(&D_008102B0, arg0, st) == 0 && func_001A7B80(arg0) != 0) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            D_008102BF = 2;
            *(int *)0x700038A0 = 0;
            *(int *)0x700038A4 = 0;
            *(float *)0x700038A8 = 1.0f;
            *(float *)0x700038AC = 1.0f;
            func_001B2B10(arg0, D_700038A0, D_700038A0);
            f0 = *(float *)0x700038A0;
            f1 = *(float *)0x700038A4;
            f2 = *(float *)0x700038A8;
            f3 = *(float *)0x700038AC;
            D_00810320 = f0;
            D_00810324 = f1;
            D_00810328 = f2;
            D_0081032C = f3;
            fl = *(unsigned char *)(arg0 + 0xD);
            if (fl == 3) {
                if (D_0081070A != 0) {
                    D_008104D4 = 0x41C80000;
                } else {
                    D_008104D4 = 0x41A00000;
                }
            } else {
                if (fl & 0x80) {
                    if (D_0081070A != 0) {
                        D_008104D4 = 0x41F00000;
                    } else {
                        D_008104D4 = 0x41E00000;
                    }
                } else {
                    D_008104D4 = 0x41A00000;
                }
            }
            D_008102B0 |= 2;
            func_001B55E0(arg0, 1);
            anim_clip_init(arg0, 0x21, 1.0f, 0.0f);
        } else {
            *(int *)0x700038A0 = 0;
            *(float *)0x700038A4 = 3.0f;
            *(float *)0x700038A8 = 10.0f;
            *(float *)0x700038AC = 1.0f;
            func_00131F20(arg0, D_700038A0, D_700038A0);
            func_001028B8(D_700038A0, D_700038A0, arg0 + 0xB0);
            if (func_001B3250(arg0, D_700038A0, 15.0f) != 0) {
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                anim_clip_init(arg0, 0xA, 3.0f, 0.0f);
            } else if (func_001B1560(arg0, &D_00810360, 1.4835298f) == 0) {
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                anim_clip_init(arg0, 0x21, 1.0f, 0.0f);
            }
        }
        break;
    case 4:
        if (*(unsigned short *)(arg1 + 0x58) & 0x1000) {
            *(char *)(arg0 + 5) = 0;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg1 + 0x62) = 0;
            *(float *)(arg1 + 0x34) = 1.0f;
            *(short *)(arg1 + 0x5A) = 0;
            *(char *)(arg1 + 0x6B) = 0;
        }
        break;
    }
    if ((int)(*(unsigned char *)(arg0 + 6)) >= 2) {
        if (*(unsigned char *)(arg1 + 0x69) & 1) {
            c = *(unsigned short *)(arg1 + 0x50);
            if (c != 0) {
                *(short *)(arg1 + 0x50) = c - 1;
            } else {
                *(short *)(arg1 + 0x50) = ((func_00122BB8(arg0) >> 9) & 7) + 5;
                *(float *)0x700038A0 = *(float *)(arg0 + 0xB0);
                *(float *)0x700038A4 = *(float *)(arg0 + 0xB4);
                *(float *)0x700038A8 = *(float *)(arg0 + 0xB8);
                *(float *)0x700038AC = 1.0f;
                *(float *)0x700038A4 = *(float *)(arg1 + 0x40);
                func_001EFD90(0x8000001D, D_700038A0, arg0 + 0xC0);
            }
        }
    }
    func_00132490(arg0, arg1);
}

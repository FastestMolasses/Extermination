// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-state animation driver dispatched on the state byte at arg0+6 (switch 0/1/2/3),
// sibling of func_00134DC0's family. arg0 = entity, arg1 = linked state block. State 0:
// if arg1+0x5B lacks flag bits 0xC, force state=3; otherwise advance state, set
// arg1+0x5D=1, zero arg1+0x2C and arg1+0x30, and kick clip 0x17 via
// anim_clip_init(self,0x17,5.0f,0.0f). State 1: if enabled (arg1+0 & 0x1000), advance
// state, zero arg1+0x4/arg1+0x8, then start clip 0x18 or 0x19 based on a coin-flip bit
// from func_00122BB8() (>>11 & 1). States 1 and 2 both fall through to a shared trailer
// that runs func_001B1240(self+0xB0, D_00810360, D_00810368) -> arg1+0x24 and
// func_001B12B0(arg1+0x24, self+0xC4, pi/36) -> self+0xC4. State 2: if arg1+0x4==0,
// pick a threshold (0x23 if the low 15 bits of self+0x2C == 0x18, else 0x27) and, once
// self+0x3C drops to/below it, set arg1+0x4=1, honor the 0x4/0x8 flags at arg1+0x5B via
// func_001370C0, then fire func_001FBD50(self,0x851,0,450.0f). If enabled: roll another
// func_00122BB8() bit (>>5 & 1) and bump/store arg1+0x8; while that counter stays under
// 6, reset arg1+0x4=0 and restart clip 0x18/0x19 (another >>14 & 1 coin flip); once it
// reaches 6 (or the bit is unset), advance state and start clip 0x1A instead. State 3:
// if enabled, reset arg0+5/arg0+6 and arg1+0x5D, and stash (func_00122BB8() >> 11) &
// 0x1F into the short at arg1+0x50.
//
// Matched 100.0 with mwcc 2.3.3 (991202 reaches 88.3%). Keys: (1) the two "start clip
// 0x18 vs 0x19" call pairs need the (z, (z = 0.0f)) fp-arg-order idiom applied to
// whichever call sits in the FALSE arm relative to that block's own branch sense (state
// 1's false/0x19 arm; state 2 counter-block's true/0x18 arm) to get mwcc's f12/f13
// register choice to match; (2) the counter increment/compare/store at arg1+0x8 must be
// written as the single expression '(*(p) = *(p) + 1) < 6' (assignment-as-expression, no
// separate reload) so mwcc reuses the already-computed value for the delay-slot store
// instead of re-loading it.
extern void anim_clip_init(int self, int clip, float a, float b);
extern void func_001370C0(int a, int b);
extern void func_001FBD50(char *p, int a, int b, float f);
extern int func_00122BB8(void);
extern float func_001B1240(int p, float a, float b);
extern float func_001B12B0(float a, float b, float c);
extern float D_00810360;
extern float D_00810368;

void func_00134FF0(char *arg0, char *arg1) {
    unsigned char st;
    int run_trailer;
    int lim;
    float z;

    st = *(unsigned char *)(arg0 + 6);
    run_trailer = 0;
    switch (st) {
    case 0:
        if (!(*(char *)(arg1 + 0x5B) & 0xC)) {
            *(char *)(arg0 + 6) = 3;
            break;
        }
        *(char *)(arg0 + 6) = st + 1;
        *(char *)(arg1 + 0x5D) = 1;
        *(int *)(arg1 + 0x2C) = 0;
        *(int *)(arg1 + 0x30) = 0;
        anim_clip_init((int)arg0, 0x17, 5.0f, 0.0f);
        break;
    case 1:
        run_trailer = 1;
        if (*(int *)(arg1 + 0) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(int *)(arg1 + 0x4) = 0;
            *(int *)(arg1 + 0x8) = 0;
            if ((func_00122BB8() >> 11) & 1) {
                anim_clip_init((int)arg0, 0x18, 0.0f, 0.0f);
            } else {
                anim_clip_init((int)arg0, 0x19, z, (z = 0.0f));
            }
        }
        break;
    case 2:
        if (*(int *)(arg1 + 0x4) == 0) {
            run_trailer = 1;
            if ((*(short *)(arg0 + 0x2C) & ~0x8000) == 0x18) {
                lim = 0x23;
            } else {
                lim = 0x27;
            }
            if (*(float *)(arg0 + 0x3C) <= (float)lim) {
                *(int *)(arg1 + 0x4) = 1;
                if (*(char *)(arg1 + 0x5B) & 4) {
                    func_001370C0(0x2A, 1);
                }
                if (*(char *)(arg1 + 0x5B) & 8) {
                    func_001370C0(0x26, 0);
                }
                func_001FBD50(arg0, 0x851, 0, 450.0f);
            }
        }
        if (*(int *)(arg1 + 0) & 0x1000) {
            if (((func_00122BB8() >> 5) & 1) &&
                (*(int *)(arg1 + 0x8) = *(int *)(arg1 + 0x8) + 1) < 6) {
                *(int *)(arg1 + 0x4) = 0;
                if ((func_00122BB8() >> 14) & 1) {
                    anim_clip_init((int)arg0, 0x18, z, (z = 0.0f));
                } else {
                    anim_clip_init((int)arg0, 0x19, 0.0f, 0.0f);
                }
            } else {
                *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
                anim_clip_init((int)arg0, 0x1A, 0.0f, 0.0f);
            }
        }
        break;
    case 3:
        if (*(int *)(arg1 + 0) & 0x1000) {
            *(char *)(arg0 + 5) = 0;
            *(char *)(arg0 + 6) = 0;
            *(char *)(arg1 + 0x5D) = 0;
            *(short *)(arg1 + 0x50) = (func_00122BB8() >> 11) & 0x1F;
        }
        break;
    }
    if (run_trailer) {
        *(float *)(arg1 + 0x24) = func_001B1240((int)arg0 + 0xB0, D_00810360, D_00810368);
        *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x24), *(float *)(arg0 + 0xC4), 0.0872664675116539f);
    }
}

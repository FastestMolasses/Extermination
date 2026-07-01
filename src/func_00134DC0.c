// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-state animation driver dispatched on the state byte at arg0+6 (switch
// 0/1/2/3). arg0 = entity, arg1 = linked state block. State 0: if the flag byte
// arg1+0x5B has bits 0xC set, advance the state, set arg1+0x5D=1, zero arg1+0x2C
// and arg1+0x30, and kick clip 0x14 via anim_clip_init(self,0x14,5.0f,0.0f);
// otherwise force state=3. State 1: run the two motion helpers
// func_001B1240(self+0xB0, D_00810360, D_00810368) -> arg1+0x24 and
// func_001B12B0(arg1+0x24, self+0xC4, 0.05235988f /*pi/60*/) -> self+0xC4; if the
// self+0x2C flag lacks 0x8000 and self+0x3C == 7.0f, fire func_001FBD50(self,0x850,0,
// 450.0f); if self is enabled (arg1+0 & 0x1000) advance state and start clip 0x15.
// State 2: if arg1+0 == 5, honor the 0x4/0x8 flags at arg1+0x5B via func_001370C0
// then func_001FBD50(self,0x851,0,450.0f); if enabled, advance state and start clip
// 0x16. State 3: if enabled, reset bytes arg0+5/arg0+6 and arg1+0x5D, and stash
// (func_00122BB8() >> 11) & 0x1F into the short at arg1+0x50.
//
// Matched 100.0 with mwcc 2.3.3 (991202 reaches 86.8%). Keys: (1) ascending switch
// so mwcc reverses to the target's 3->2->1->0 dispatch with branch-likely;
// (2) func_001B1240/func_001B12B0 real signatures carry the two/one extra float
// args (D_00810360/D_00810368 and the 0.05235988f constant); (3) in state 0 both
// zero-stores (arg1+0x2C then arg1+0x30) are placed BEFORE the anim_clip_init call
// with 0x30 last, letting mwcc pull the 0x30 store into the jal delay slot while
// keeping the 0x2C store in the FP-arg (lui/mtc1) scheduling bubble.
extern void anim_clip_init(int self, int clip, float a, float b);
extern float func_001B1240(int p, float a, float b);
extern float func_001B12B0(float a, float b, float c);
extern void func_001FBD50(char *p, int a, int b, float f);
extern void func_001370C0(int a, int b);
extern int func_00122BB8(void);
extern float D_00810360;
extern float D_00810368;

void func_00134DC0(char *arg0, char *arg1) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 6);
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
        anim_clip_init((int)arg0, 0x14, 5.0f, 0.0f);
        break;
    case 1:
        *(float *)(arg1 + 0x24) = func_001B1240((int)arg0 + 0xB0, D_00810360, D_00810368);
        *(float *)(arg0 + 0xC4) = func_001B12B0(*(float *)(arg1 + 0x24), *(float *)(arg0 + 0xC4), 0.05235988f);
        if (!(*(short *)(arg0 + 0x2C) & 0x8000) && *(float *)(arg0 + 0x3C) == 7.0f) {
            func_001FBD50(arg0, 0x850, 0, 450.0f);
        }
        if (*(int *)(arg1 + 0) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            anim_clip_init((int)arg0, 0x15, 0.0f, 0.0f);
        }
        break;
    case 2:
        if (*(unsigned char *)(arg1 + 0) == 5) {
            if (*(char *)(arg1 + 0x5B) & 0x4) {
                func_001370C0(0x2A, 1);
            }
            if (*(char *)(arg1 + 0x5B) & 0x8) {
                func_001370C0(0x26, 1);
            }
            func_001FBD50(arg0, 0x851, 0, 450.0f);
        }
        if (*(int *)(arg1 + 0) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = *(unsigned char *)(arg0 + 6) + 1;
            anim_clip_init((int)arg0, 0x16, 0.0f, 0.0f);
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
}

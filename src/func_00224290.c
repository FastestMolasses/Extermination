// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Player sub-state +7 of a push / pull move, the sibling of func_002243F0
// (beat 10, the cage roof).
// 0: when either speed +0x224 / +0x22C is non-zero, play sound 0x152 (with
//    func_0021C350) for +0x224 and 0x153 (with func_0021C270) for +0x22C, at
//    distance 300.0; advance +7, rumble (func_001B61C0(0, 0xC0, 5, 1)) and
//    start clip 0x76 (func_001749A0(e, 0x76, 0, 1.0)); returns 1, or 0 when
//    both speeds are zero.
// 1: when bit 0x1000 of +0x200 is set, back to sub-state 0, start clip 0x72
//    and set the timer +0x20E = 60. Returns 1.
// Any other sub-state returns 1.
extern void func_001FBD50(char *e, int sound, int b, float dist);
extern void func_0021C350(char *e);
extern void func_0021C270(char *e);
extern void func_001B61C0(int port, int strength, int mode, int arg3);
extern int func_001749A0(char *e, short clip, int flags, float blend);

int func_00224290(char *e) {
    switch ((unsigned char)e[7]) {
    case 0:
        if (*(float *)(e + 0x224) || *(float *)(e + 0x22C)) {
            if (*(float *)(e + 0x224)) {
                func_001FBD50(e, 0x152, 0, 300.0f);
                func_0021C350(e);
            }
            if (*(float *)(e + 0x22C)) {
                func_001FBD50(e, 0x153, 0, 300.0f);
                func_0021C270(e);
            }
            ((unsigned char *)e)[7]++;
            func_001B61C0(0, 0xC0, 5, 1);
            func_001749A0(e, 0x76, 0, 1.0f);
            break;
        }
        return 0;
    case 1:
        if (*(int *)(e + 0x200) & 0x1000) {
            e[7] = 0;
            func_001749A0(e, 0x72, 0, 1.0f);
            *(short *)(e + 0x20E) = 0x3C;
        }
        break;
    }
    return 1;
}

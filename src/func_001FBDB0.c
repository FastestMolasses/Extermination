// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Positions a playing sound voice by its distance to the listener. voice is
// the sound handle (-1 = none: returns -1). Only when func_00119890(1, voice)
// reports 2 (entry voice of the 0x78-byte voice table at D_0027E0C0: +0x2E == 0
// and +0x32 == 1) does it ask func_001FBF50(actor, &vol, &pan, 0,
// range, falloff) for the distance attenuation; a non-zero result sets the
// voice with func_0011A218(voice, vol, pan) and returns voice. When the source
// is out of range the voice is stopped with func_0011A070(voice). Every other
// path returns -1.
extern int func_00119890(int port, int voice);
extern void func_0011A070(int voice);
extern void func_0011A218(int voice, int vol, int pan);
extern int func_001FBF50(char *actor, int *vol, int *pan, int mode, float range, float falloff);

int func_001FBDB0(char *actor, int voice, float range, float falloff) {
    int vol;
    int pan;

    if (voice == -1) {
        return -1;
    }
    if (func_00119890(1, voice) != 2) {
        return -1;
    }
    if (func_001FBF50(actor, &vol, &pan, 0, range, falloff)) {
        func_0011A218(voice, vol, pan);
    } else {
        func_0011A070(voice);
        return -1;
    }
    return voice;
}

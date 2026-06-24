// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Re-trigger the entity's current animation clip: read the live clip id at
// +0x20C and, depending on which "looping" clip it currently matches, restart
// the paired clip at full (1.0) blend. Mirror of func_0015C7C0 (the two
// functions restart opposite members of each clip pair).
extern int func_001749A0(unsigned char *e, short clip, int flags, float blend);

extern short D_00248A00;
extern short D_00248A02;
extern short D_00248A04;
extern short D_00248A06;
extern short D_00248A08;
extern short D_002754C0;
extern short D_002754C2;
extern short D_002754C4;
extern short D_002754C6;
extern short D_002754C8;
extern short D_002754CA;
extern short D_002754CC;
extern short D_002754CE;
extern short D_002754D0;
extern short D_002754D2;
extern short D_002754D4;
extern short D_002754D6;
extern short D_002754D8;
extern short D_002754DA;

void func_0015C9D0(unsigned char *e) {
    short clip = *(short *)(e + 0x20C);

    if (clip == D_00248A00 || clip == D_00248A08) {
        func_001749A0(e, D_00248A02, 0, 1.0f);
        return;
    }
    if (clip == D_00248A04) {
        func_001749A0(e, D_00248A06, 0, 1.0f);
        return;
    }
    if (clip == D_002754C0) {
        func_001749A0(e, D_002754C2, 0, 1.0f);
        return;
    }
    if (clip == D_002754C4) {
        func_001749A0(e, D_002754C6, 0, 1.0f);
        return;
    }
    if (clip == D_002754C8) {
        func_001749A0(e, D_002754CA, 0, 1.0f);
        return;
    }
    if (clip == D_002754CC) {
        func_001749A0(e, D_002754CE, 0, 1.0f);
        return;
    }
    if (clip == D_002754D0) {
        func_001749A0(e, D_002754D2, 0, 1.0f);
        return;
    }
    if (clip == D_002754D4) {
        func_001749A0(e, D_002754D6, 0, 1.0f);
        return;
    }
    if (clip == D_002754D8) {
        func_001749A0(e, D_002754DA, 0, 1.0f);
    }
}

// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Re-trigger the entity's current animation clip: read the live clip id at
// +0x20C and, depending on which "looping" clip it currently matches, restart
// the corresponding clip at full (1.0) blend. The first family member has an
// alternate variant chosen by a global flag (bit 0x4 of func_001B0070()).
extern int func_001749A0(unsigned char *e, short clip, int flags, float blend);
extern int func_001B0070(void);

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

void func_0015C7C0(unsigned char *e) {
    short clip = *(short *)(e + 0x20C);

    if (clip == D_00248A02) {
        if (func_001B0070() & 4) {
            func_001749A0(e, D_00248A08, 0, 1.0f);
            return;
        }
        func_001749A0(e, D_00248A00, 0, 1.0f);
        return;
    }
    if (clip == D_00248A06) {
        func_001749A0(e, D_00248A04, 0, 1.0f);
        return;
    }
    if (clip == D_002754C2) {
        func_001749A0(e, D_002754C0, 0, 1.0f);
        return;
    }
    if (clip == D_002754C6) {
        func_001749A0(e, D_002754C4, 0, 1.0f);
        return;
    }
    if (clip == D_002754CA) {
        func_001749A0(e, D_002754C8, 0, 1.0f);
        return;
    }
    if (clip == D_002754CE) {
        func_001749A0(e, D_002754CC, 0, 1.0f);
        return;
    }
    if (clip == D_002754D2) {
        func_001749A0(e, D_002754D0, 0, 1.0f);
        return;
    }
    if (clip == D_002754D6) {
        func_001749A0(e, D_002754D4, 0, 1.0f);
        return;
    }
    if (clip == D_002754DA) {
        func_001749A0(e, D_002754D8, 0, 1.0f);
    }
}

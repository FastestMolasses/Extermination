// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// Player check: when the actor has a non-zero +0x224 or +0x22C speed or bit 1
// of flag byte +0xF set, it switches to state (2, 4, 0) (bytes +4/+5/+6),
// stores code in +0x302 and returns 1; otherwise returns 0.
int func_00181110(unsigned char *actor, char code) {
    if (*(float *)(actor + 0x224) || *(float *)(actor + 0x22C) || (actor[0xF] & 2)) {
        actor[4] = 2;
        actor[5] = 4;
        actor[6] = 0;
        actor[0x302] = code;
        return 1;
    }
    return 0;
}

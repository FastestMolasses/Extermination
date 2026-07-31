// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 4
// Per-frame state driver for actor arg0, dispatched on the state byte at arg0+6:
// state 0 spawns (advances state, fires cue 0x3E/0x5D via func_001749A0, resets the
// 0x38/0x21C/0x2E4 fields, sets the 0x25C flag, kicks func_001FBD50); state 1 advances
// once the 0x3C timer falls to <= 10.0f and falls through into state 2; state 2 either
// tears down (0x1000 bit of arg0+0x200) or re-bases the 0x38 timer off the global clock
// at (*D_00275B40)[8]. All states then run the common tail: update, 0xB4 -= 0.2f,
// animate, post, and latch arg0[0] = 1 when arg0[4] == 1 && arg0[5] != 0x24.

extern void func_001749A0(char *p, int code, int flags, float blend);
extern void func_00175900(char *p, int a);
extern void func_001764E0(char *p);
extern void func_00178B90(char *p, int a);
extern void func_001796C0(char *p);
extern void func_001FBD50(char *p, int a, int b, float f);
extern char **D_00275B40;

void func_001747F0(char *arg0) {
    unsigned char st;

    st = *(unsigned char *)(arg0 + 6);
    switch (st) {
    case 0:
        *(unsigned char *)(arg0 + 6) = st + 1;
        *(char *)(arg0 + 7) = 0;
        if (*(unsigned char *)(arg0 + 0x236) == 0) {
            func_001749A0(arg0, 0x3E, 0, 0.0f);
        } else {
            func_001749A0(arg0, 0x5D, 0, 0.0f);
        }
        *(int *)(arg0 + 0x38) = 0;
        *(char *)(arg0 + 0x25C) = 1;
        *(int *)(arg0 + 0x21C) = 0;
        *(int *)(arg0 + 0x2E4) = 0;
        func_001FBD50(arg0, 0x186, 0, 300.0f);
        break;
    case 1:
        if (*(float *)(arg0 + 0x3C) <= 10.0f) {
            *(unsigned char *)(arg0 + 6) = st + 1;
            *(char *)(arg0 + 0) = 1;
        }
        /* fallthrough */
    case 2:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(char *)(arg0 + 5) = 0;
            *(char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0x1F0) = 0;
        } else {
            *(float *)(arg0 + 0x38) = *(float *)(*D_00275B40 + 8) - *(float *)(arg0 + 0x21C);
            *(float *)(arg0 + 0x21C) = *(float *)(*D_00275B40 + 8);
            func_00178B90(arg0, 0);
        }
        break;
    }
    func_001764E0(arg0);
    *(float *)(arg0 + 0xB4) += -0.2f;
    func_00175900(arg0, 1);
    func_001796C0(arg0);
    if (*(unsigned char *)(arg0 + 4) == 1 && *(unsigned char *)(arg0 + 5) != 0x24 && *(unsigned char *)(arg0 + 0) != 1) {
        *(char *)(arg0 + 0) = 1;
    }
}

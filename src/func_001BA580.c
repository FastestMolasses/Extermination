// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Per-frame face update for a scripted body (called by func_001BB0E0).
// Types 0x6A / 0x6C only run func_001DA6A0. Otherwise the body type picks
// its speaker slot, the D_008106D4 face-talk mailbox index (-1: none). With
// the face attached (+0x56 != 0) and a slot, func_001DA6A0 runs (for type
// 0x61 while D_00810700 is 0xD, func_001BA7F0 instead); a mailbox value of
// 1 or 2 calls func_001D06E0 with 1 or 0 and clears the mailbox; then
// func_001D0C70 runs. The table was read off the original's compare chain
// and agrees with the port's em_roger_actor_original.c. `type` is an int:
// the original compares the whole register.

extern void func_001DA6A0(int a0);
extern void func_001BA7F0(unsigned char *a0);
extern void func_001D06E0();
extern void func_001D0C70();
extern unsigned char D_00810700;
extern char D_008106D4[];

void func_001BA580(unsigned char *self, int type) {
    int slot;
    int alt;
    unsigned char *p;
    unsigned char v;

    alt = 0;
    switch (type) {
    case 0x3B:
    case 0x3E:
    case 0x3F:
    case 0x40: slot = 0; break;
    case 0x47:
    case 0x48:
    case 0x49: slot = 1; break;
    case 0x4F:
    case 0x50:
    case 0x51: slot = 2; break;
    case 0x54:
    case 0x55: slot = 3; break;
    case 0x59:
    case 0x5A: slot = 4; break;
    case 0x5D:
    case 0x5E: slot = 5; break;
    case 0x61:
        slot = 6;
        if (D_00810700 == 0xD) {
            alt = 1;
        }
        break;
    case 0x64: slot = 7; break;
    case 0x66: slot = 8; break;
    case 0x68: slot = 9; break;
    case 0x6A:
    case 0x6C:
        func_001DA6A0((int)self);
        return;
    default:
        slot = -1;
        break;
    }

    if (*(short *)(self + 0x56) != 0 && slot != -1) {
        if (alt == 0) {
            func_001DA6A0((int)self);
        } else {
            func_001BA7F0(self);
        }
        p = (unsigned char *)(D_008106D4 + slot);
        v = *p;
        if (v == 1) {
            func_001D06E0(self, 1);
            *p = 0;
        } else if (v == 2) {
            func_001D06E0(self, 0);
            *p = 0;
        }
        func_001D0C70(self);
    }
}

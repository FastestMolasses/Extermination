// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// First-tick face attach for a scripted body. func_001F0120(self, type) runs
// unless event flag 0x30 (D_00810788) is 1 and the type is not 0x3B; then
// func_001D8BF0(self, 1) runs whatever the type. The body type picks the
// face D_0028A490[face], attached at head node `node` by func_001CA700, the
// shadow kind at +0x96 and the func_001D06D0 flag. 0x6A / 0x6C only set the
// shadow kind; an unknown type leaves +0x96 alone. +0x56 ends 1 when the
// face attached, else 0. The table was read off the original's compare chain
// (each case's values sit in its branch delay slot) and agrees with the
// port's em_roger_actor_original.c.

extern unsigned char D_00810788;
extern int D_0028A490[];
extern void func_001F0120(void *a0, int a1);
extern void func_001D8BF0(void *a0, int a1);
extern int func_001CA700(int p, int arg, int n);
extern void func_001D06D0(int p, int flag);

void func_001BA8E0(unsigned char *self, int type) {
    int face;
    int node;
    int shadow;
    int flag;

    if (D_00810788 != 1 || type == 0x3B) {
        func_001F0120(self, type);
    }
    func_001D8BF0(self, 1);

    flag = 1;
    shadow = 0;

    switch (type) {
    case 0x3B: face = 0x18; node = 7; break;
    case 0x3E: face = 0x19; node = 7; break;
    case 0x3F: face = 0x1A; node = 7; break;
    case 0x40: face = 0x1B; node = 7; break;
    case 0x47:
    case 0x48:
    case 0x49: face = 0x88; node = 7; shadow = 0x29; break;
    case 0x4F: face = 0x8E; node = 7; shadow = 0x2A; break;
    case 0x50: face = 0x93; node = 7; shadow = 0x2A; break;
    case 0x51: face = 0x95; node = 7; shadow = 0x2A; break;
    case 0x54: face = 0x89; node = 7; shadow = 0x2B; break;
    case 0x55: face = 0x8F; node = 7; shadow = 0x2B; break;
    case 0x59: face = 0x90; node = 7; shadow = 0x2C; flag = 0; break;
    case 0x5A: face = 0x94; node = 7; shadow = 0x2C; flag = 0; break;
    case 0x5D: face = 0x8A; node = 7; shadow = 0x2D; break;
    case 0x5E: face = 0x91; node = 7; shadow = 0x2D; break;
    case 0x61: face = 0x8B; node = 6; shadow = 0x2E; break;
    case 0x64: face = 0x8C; node = 6; shadow = 0x2F; break;
    case 0x66: face = 0x92; node = 6; shadow = 0x30; break;
    case 0x68: face = 0x8D; node = 6; shadow = 0x31; flag = 0; break;
    case 0x6A:
        *(unsigned short *)(self + 0x96) = 0x33;
        *(unsigned short *)(self + 0x56) = 0;
        return;
    case 0x6C:
        *(unsigned short *)(self + 0x96) = 0x34;
        *(unsigned short *)(self + 0x56) = 0;
        return;
    default:
        face = -1;
        break;
    }

    if (face == -1) {
        *(unsigned short *)(self + 0x56) = 0;
        return;
    }

    *(unsigned short *)(self + 0x96) = shadow;
    if (func_001CA700((int)self, D_0028A490[face], node) != 0) {
        func_001D06D0((int)self, flag);
        *(unsigned short *)(self + 0x56) = 1;
        return;
    }
    *(unsigned short *)(self + 0x56) = 0;
}

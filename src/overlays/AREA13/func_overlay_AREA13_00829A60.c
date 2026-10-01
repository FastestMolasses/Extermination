// NEARMISS func_overlay_AREA13_00829A60 (94.23%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA13 overlay, runtime 0x00829AA0 (splat/link name 00829A60; overlay code
//  is linked 0x40 below where it runs), 0x1F8 bytes.
// Role (read from the instructions): a GS packet (no static reach): state 0
//  sets a 24-frame count in +5; each frame writes a packet from
//  func_001CB5F0(D_007635C0, 0xFFE000, 6) whose colour word is
//  func_001281C0(224 * count / 24) in bytes 0..2 and func_001281C0(128) in
//  byte 3; state 3 at zero.
// Divergence: the state byte is loaded into a2 before the self copy in the
//  original (the dispatch then fills the case-3 slot with the copy and uses
//  branch-likely on case 2); mwcc 2.3.3 copies self first and loads into a0.
//  This is the wall recorded for AREA16 0x8237A0; a switch local, extra
//  parameters and declaration orders do not move it.
typedef unsigned __int128 u128;
extern char D_007635C0[];
extern int func_001281C0(float v);
extern char *func_001CB5F0(char *a0, int a1, int a2);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA13_00829A60(unsigned char *self) {
    float f;
    int c;
    char *p;
    switch (self[4]) {
    case 0:
        self[4] = 1;
        self[5] = 0x18;
    case 1:
        *(float *)0x70003A20 = (float)self[5] / 24.0f;
        f = 224.0f * *(float *)0x70003A20;
        c = func_001281C0(f);
        c |= func_001281C0(f) << 8;
        c |= func_001281C0(f) << 16;
        c |= func_001281C0(128.0f) << 24;
        p = func_001CB5F0(D_007635C0, 0xFFE000, 6);
        *(u128 *)(p + 0x0) = 0;
        *(int *)(p + 0xC) = 0x50000005;
        *(long long *)(p + 0x10) = (long long)0x8001 | ((long long)0x10000000 << 32);
        *(long long *)(p + 0x18) = 0xE;
        *(long long *)(p + 0x20) = (long long)0x68 | ((long long)0x80 << 32);
        *(long long *)(p + 0x28) = 0x42;
        *(long long *)(p + 0x30) = (long long)0x8001 | ((long long)0x44000000 << 32);
        *(long long *)(p + 0x38) = 0x4410;
        *(long long *)(p + 0x40) = 0x46;
        *(long long *)(p + 0x48) = (long long)c;
        *(long long *)(p + 0x50) = (long long)0x79007000 | ((long long)0xFFFFFF << 32);
        *(long long *)(p + 0x58) = (long long)(int)0x87009000;
        self[5]--;
        if (self[5] == 0) {
            self[4] = 3;
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}

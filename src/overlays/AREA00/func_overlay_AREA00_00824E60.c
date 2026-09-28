// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA00 overlay, runtime 0x00824EA0 (splat/link name 00824E60; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%,
// tools/overlay/overlay_match.py check AREA00).
// Role: state 0 leaves for state 3 when func_001BA1C0(self, 2) or
//  D_00810701 is set; otherwise, once func_001B10B0(self, 0xF, 0x11) returns 0,
//  it seeds the +0x1F0 block and the +0xD0 matrix, then waits on
//  func_00129780(self, block, 3) to start script 0x828D60. State 1 runs
//  func_001C2770 / func_001C3D60 each frame, counts D_70003B84 (sub 0 clears
//  it), steps +5 at 330 and when the script ends (then state 2); sub 2 calls
//  func_001B1E20(6, 0) at 1200 when D_70003B8D is set. State 2 calls
//  func_001C4760(3, 1).
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_00810701;
extern unsigned char D_70003B8D;
extern char D_70003000[];
extern char D_70003400[];
extern char D_overlay_AREA00_00828D60[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern int func_001B10B0(unsigned char *self, int a1, int a2);
extern void func_001C63E0(unsigned char *self, short a1);
extern void func_001029C0(void *m);
extern int func_00129780(unsigned char *a, unsigned char *b, unsigned char sel);
extern void func_001BA1A0(unsigned char *, unsigned char *);
extern int func_001C2770(void *a0, void *a1, int a2);
extern void func_001C3D60(void *a, void *b);
extern short func_001C64F0(unsigned char *self, float dt);
extern void func_00102958(void *dst, void *src);
extern void func_001C69A0(unsigned char *self);
extern int func_001BA1F0(unsigned char *self);
extern void func_001B1E20(int cue, int dur);
extern int func_001C4760(int a0, int a1);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA00_00824E60(unsigned char *self) {
    unsigned char *sub = self + 0x1F0;
    switch (self[4]) {
    case 0:
        switch (self[5]) {
        case 0:
            if (func_001BA1C0(self, 2) != 0) {
                self[4] = 3;
                break;
            }
            if (D_00810701 != 0) {
                self[4] = 3;
                break;
            }
            if (func_001B10B0(self, 0xF, 0x11) == 0) {
                func_001C63E0(self, 1);
                *(float *)(sub + 0xEC) = 1.0f;
                *(float *)(sub + 0xD8) = 0.0f;
                *(float *)(sub + 0xE4) = 0.0f;
                *(float *)(sub + 0xDC) = 0.0f;
                *(float *)(sub + 0x60) = 0.0f;
                *(float *)(sub + 0x64) = -0.4f;
                *(float *)(sub + 0x68) = 0.0f;
                *(float *)(sub + 0x6C) = 1.0f;
                *(float *)(sub + 0x70) = 0.0f;
                *(float *)(sub + 0x74) = 0.0f;
                *(float *)(sub + 0x78) = 1.0f;
                *(float *)(sub + 0x7C) = 1.0f;
                *(float *)(sub + 0x80) = 0.0f;
                *(float *)(sub + 0x84) = 1.0f;
                *(float *)(sub + 0x88) = 0.0f;
                *(float *)(sub + 0x8C) = 1.0f;
                func_001029C0(self + 0xD0);
                self[5]++;
            }
            break;
        case 1:
            if (func_00129780(self, sub, 3) != 0) {
                func_001BA1A0(sub, (unsigned char *)D_overlay_AREA00_00828D60);
            }
            break;
        }
        break;
    case 1:
        func_001029C0(D_70003000);
        switch (self[5]) {
        case 0:
            self[5]++;
            *(unsigned short *)0x70003B84 = 0;
        case 1:
            if (func_001C2770(self, sub, 0) == 0) {
                func_001C3D60(self, sub);
            }
            func_001C64F0(self, *(float *)(sub + 0xEC));
            func_00102958(D_70003400, D_70003000);
            func_001C69A0(self);
            (*(ActorFn *)(self + 0x4C))(self);
            if (*(unsigned short *)0x70003B84 >= 0x14A) {
                self[5]++;
            }
            if (func_001BA1F0(self) != 0) {
                self[4]++;
                self[5] = 0;
            }
            break;
        case 2:
            if (func_001BA1F0(self) != 0) {
                self[4]++;
                self[5] = 0;
            }
            if (D_70003B8D != 0 && *(unsigned short *)0x70003B84 == 0x4B0) {
                func_001B1E20(6, 0);
            }
            break;
        }
        break;
    case 2:
        func_001C4760(3, 1);
        self[4]++;
        break;
    case 3:
        func_001AFC10(self);
        break;
    }
}

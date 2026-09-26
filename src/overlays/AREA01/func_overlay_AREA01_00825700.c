// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA01 overlay, runtime 0x00825740 (splat/link name 00825700; overlay
// code is linked 0x40 below where it runs). Byte-identical (objdiff 100%).
// Role: sub-0 placement owner (O1); talk owner that runs script 0x82A7B0.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_0081075A;
extern int D_0028A5C4;
extern char D_overlay_AREA01_0082A8F0[];
extern char D_overlay_AREA01_0082A7B0[];
extern int func_001B10B0(unsigned char *self, int a1, int a2);
extern void func_001BA8E0(unsigned char *self, int type);
extern void func_001C63E0(unsigned char *self, short a1);
extern int func_001BA1C0(unsigned char *self, int idx);
/* unprototyped: callers pass two or three arguments */
extern void func_001BA1A0();
extern int func_001BA1F0(unsigned char *self);
extern void func_001C67E0(unsigned char *self, int clip, float blend, float frame);
extern void func_001BA580(unsigned char *self, unsigned char a1);
extern short func_001C64F0(unsigned char *self, float dt);
extern void func_001C68C0(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern void func_001BA540(unsigned char *self);
extern void func_001AFC10(unsigned char *self);

void func_overlay_AREA01_00825700(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[4]) {
    case 0:
        func_001B10B0(self, self[0xD], 0x4A);
        func_001BA8E0(self, self[0xD]);
        func_001C63E0(self, 1);
        self[0] = 1;
        *(short *)(self + 0x28) = 0;
        *(char **)(self + 0x30) = D_overlay_AREA01_0082A8F0;
        *(float *)(talk + 0x44) = *(float *)(self + 0xC4);
        *(int *)(self + 0x58) = D_0028A5C4;
        if (D_0081075A == 0) {
            self[4] = 3;
            break;
        }
        if (func_001BA1C0(self, 6) != 0) {
            self[4] = 3;
            break;
        }
        self[4] = 1;
        break;
    case 1:
        switch (self[5]) {
        case 0:
            if (self[0xB] & 4) {
                self[5] = 1;
                func_001BA1A0(talk, D_overlay_AREA01_0082A7B0);
            }
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                self[5] = 0;
                self[0xB] = 0;
                func_001C67E0(self, 1, 30.0f, 0.0f);
            }
            break;
        }
        func_001BA580(self, self[0xD]);
        func_001C64F0(self, 1.0f);
        func_001C68C0(self);
        func_001B17A0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001BA540(self);
        func_001AFC10(self);
        break;
    }
}

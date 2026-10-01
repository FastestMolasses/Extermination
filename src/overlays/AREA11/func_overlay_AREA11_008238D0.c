// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA11 overlay, runtime 0x00823910 (splat/link name 008238D0; overlay code is
// linked 0x40 below where it runs), 0x254 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA11; lane A11C).
// Covers the splat pieces 008238D0, 00823910 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: sub-behaviour at runtime 0x823910 (called from 0x8237E0 while
// D_008107D8 == 0). D_00810791 == 1 only clears +1. While D_00810793 == 1
// and D_00810813 == 0 it is a talk loop on script 0x828990 (+5 0 starts it,
// +5 1 waits and plays clip 8 at 20.0). Otherwise: D_00810813 0x10 sets
// +0xC4 = 2.84488654 and D_00810813 = 0x11; +5 0 waits for the player
// position D_00810350 inside the 4-point area 0x82AB80 (func_001B1EA0),
// then starts script 0x8283D0 and func_001FABB0; +5 1 at the script end
// sets +0x40 = D_0028A5B8, clip 8, D_008107D8 |= 1, func_001FAE70(0),
// func_001AEE10(4, 0) and +5 = 0; the talk block +0x1FE takes
// func_001C64F0(self, 0.5). Then func_001BA580, func_001B17A0, +1 = 1,
// func_001C68C0 and the +0x4C method.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_00810791;
extern unsigned char D_00810793;
extern unsigned char D_00810813;
extern unsigned char D_008107D8;
extern int D_0028A5B8;
extern char D_00810350[];
extern char D_overlay_AREA11_00828990[];
extern char D_overlay_AREA11_008283D0[];
extern char D_overlay_AREA11_0082AB80[];
/* unprototyped: callers pass two or three arguments */
extern void func_001BA1A0();
extern int func_001BA1F0(unsigned char *self);
extern void func_001C67E0(unsigned char *self, int clip, float blend, float frame);
extern void func_001BA580(unsigned char *self, unsigned char a1);
extern short func_001C64F0(unsigned char *self, float dt);
extern void func_001C68C0(unsigned char *self);
extern int func_001B17A0(unsigned char *self);
extern int func_001B1EA0(int a0, void *pos, void *poly, int n);
extern void func_001FABB0(void);
extern void func_001FAE70(int a0);
extern void func_001AEE10(int a0, int a1);

void func_overlay_AREA11_008238D0(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    if (D_00810791 == 1) {
        self[1] = 0;
        return;
    }
    if (D_00810793 == 1 && D_00810813 == 0) {
        switch (self[5]) {
        case 0:
            func_001BA1A0(talk, D_overlay_AREA11_00828990);
            self[5] = 1;
            break;
        case 1:
            if (func_001BA1F0(self) != 0) {
                int zi = 0;
                float z = (float)zi;
                self[5] = 0;
                func_001C67E0(self, 8, 20.0f, z);
            }
            break;
        }
        func_001BA580(self, self[0xD]);
        func_001C64F0(self, 1.0f);
        func_001C68C0(self);
        func_001B17A0(self);
        (*(ActorFn *)(self + 0x4C))(self);
        return;
    }
    if (D_00810813 == 0x10) {
        *(float *)(self + 0xC4) = 2.84488654f;
        D_00810813 = 0x11;
    }
    switch (self[5]) {
    case 0:
        if (func_001B1EA0(0, D_00810350, (void *)D_overlay_AREA11_0082AB80, 4) != 0) {
            func_001BA1A0(talk, D_overlay_AREA11_008283D0);
            func_001FABB0();
            self[5] = 1;
        }
        func_001C64F0(self, 1.0f);
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            *(int *)(self + 0x40) = D_0028A5B8;
            func_001C67E0(self, 8, 0.0f, 0.0f);
            D_008107D8 |= 1;
            func_001FAE70(0);
            self[5] = 0;
            func_001AEE10(4, 0);
        }
        *(short *)(talk + 0xE) = func_001C64F0(self, 0.5f);
        break;
    }
    func_001BA580(self, self[0xD]);
    func_001B17A0(self);
    self[1] = 1;
    func_001C68C0(self);
    (*(ActorFn *)(self + 0x4C))(self);
}

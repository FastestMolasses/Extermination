// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA02 overlay, runtime 0x00824020 (splat/link name 00823FE0; overlay
// code is linked 0x40 below where it runs), 0x2C8 bytes. Byte-identical
// (tools/overlay/overlay_match.py check AREA02; lane A02C).
// Covers the splat pieces 00823FE0, 00824020 (the later piece is absorbed at
// link time, tools/overlay/fill_overlay.py).
// Role: behaviour by kind +0xD with state +4. 0: if D_00810761 is set,
//  kinds 10/11 set +5 = 1, call func_001B0FD0 and func_0019C6F0(1, 1); other
//  kinds set +4 = 3. Otherwise kinds 10/11 call func_001B0FD0, other kinds
//  need 0x824800 to return nonzero (else +4 = 3); then the 0x40 words at
//  +0x1F0 are cleared, func_00121A28(+0x2B0, 0, 0x40), +4 = 1, +0 = 1,
//  +0x28 = +0x2A = 0. 1: kinds 7/8: +5 0 waits for D_008107E1 bit 1 (+5 = 1),
//  +5 1 sets D_00810761 = D_008107E1 = 0xFF once 0x8242F0 returns nonzero.
//  Kind 10: +5 0 waits for bit 7 (func_001B6660(0x825C00), +5 = 1), +5 1
//  animates (func_001B1B70, func_001C6380, +0x4C callback). Kind 11: waits
//  for bit 6, then animates. 2, 3: func_001AFC10.
typedef void (*ActorFn)(unsigned char *);
extern unsigned char D_00810761;
extern unsigned char D_008107E1;
extern char D_overlay_AREA02_00825C00[];
extern int func_0019C6F0(int a0, int a1);
extern int func_001B0FD0(unsigned char *self);
extern void func_001B1B70(unsigned char *self);
extern void func_001C6380(unsigned char *self);
extern void func_001AFC10(unsigned char *self);
extern unsigned char *func_001B6660(void *p);
extern void func_00121A28(void *p, int a1, int a2);
extern int func_overlay_AREA02_00824800(unsigned char *self);
extern int func_overlay_AREA02_008242F0(unsigned char *self);

void func_overlay_AREA02_00823FE0(unsigned char *self) {
    int i;
    int *p;
    switch (self[4]) {
    case 0:
        if (D_00810761 != 0) {
            if (self[0xD] == 0xA || self[0xD] == 0xB) {
                self[5] = 1;
                func_001B0FD0(self);
            } else {
                self[4] = 3;
                break;
            }
            func_0019C6F0(1, 1);
            break;
        }
        if (self[0xD] == 0xA || self[0xD] == 0xB) {
            func_001B0FD0(self);
        } else if (func_overlay_AREA02_00824800(self) == 0) {
            self[4] = 3;
            break;
        }
        p = (int *)(self + 0x1F0);
        for (i = 0; i < 0x40; i++) {
            *p++ = 0;
        }
        func_00121A28(self + 0x2B0, 0, 0x40);
        self[4] = 1;
        self[0] = 1;
        *(short *)(self + 0x28) = 0;
        *(short *)(self + 0x2A) = 0;
        break;
    case 1:
        if (self[0xD] == 7 || self[0xD] == 8) {
            switch (self[5]) {
            case 0:
                if (D_008107E1 & 2) {
                    self[5] = 1;
                }
                break;
            case 1:
                if (func_overlay_AREA02_008242F0(self)) {
                    D_00810761 = 0xFF;
                    D_008107E1 = 0xFF;
                }
                break;
            }
        }
        if (self[0xD] == 0xA) {
            switch (self[5]) {
            case 0:
                if (D_008107E1 & 0x80) {
                    func_001B6660(D_overlay_AREA02_00825C00);
                    self[5] = 1;
                }
                break;
            case 1:
                func_001B1B70(self);
                func_001C6380(self);
                (*(ActorFn *)(self + 0x4C))(self);
                break;
            }
        }
        if (self[0xD] == 0xB) {
            switch (self[5]) {
            case 0:
                if (D_008107E1 & 0x40) {
                    self[5] = 1;
                }
                break;
            case 1:
                func_001B1B70(self);
                func_001C6380(self);
                (*(ActorFn *)(self + 0x4C))(self);
                break;
            }
        }
        break;
    case 2:
    case 3:
        func_001AFC10(self);
        break;
    }
}

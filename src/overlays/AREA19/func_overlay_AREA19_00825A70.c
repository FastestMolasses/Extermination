// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA19 overlay, runtime 0x00825AB0 (splat/link name 00825A70; overlay code
//  is linked 0x40 below where it runs), 0x1B8 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA19; lane A13C).
// Covers the splat pieces 00825A70, 00825AB0 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: sub-state of 0x8257C0 (D_008107F6 == 0). +5 0: script 0x82C410 when
//  D_00810702 == 0xA; +5 1: when it ends +0x28 = 0, func_001831F0(0), script
//  0x82C4D0, +7 = D_008102B5, D_008102B5 = 0; func_001831F0(2) every frame of
//  +5 1; +5 2: +5 = 3 when the script ends, counts +0x28 and from 500 on
//  (restoring D_008102B5 at 500) calls func_001831F0(2); +5 3: script
//  0x82C690 once D_008102B5 == 0, else func_001831F0(2); +5 4: D_008107F6 = 1
//  when it ends.
extern unsigned char D_00810702;
extern unsigned char D_008102B5;
extern char D_overlay_AREA19_0082C410[];
extern char D_overlay_AREA19_0082C4D0[];
extern char D_overlay_AREA19_0082C690[];
extern void func_001BA1A0(unsigned char *talk, void *script);
extern int func_001BA1F0(unsigned char *self);
extern void func_001831F0(int mode);

void func_overlay_AREA19_00825A70(unsigned char *self) {
    unsigned char *talk = self + 0x1F0;
    switch (self[5]) {
    case 0:
        if (D_00810702 == 0xA) {
            func_001BA1A0(talk, D_overlay_AREA19_0082C410);
            self[5] = 1;
        }
        break;
    case 1:
        if (func_001BA1F0(self) != 0) {
            self[5] = 2;
            *(short *)(self + 0x28) = 0;
            func_001831F0(0);
            func_001BA1A0(talk, D_overlay_AREA19_0082C4D0);
            self[7] = *(unsigned char *)0x8102B5;
            *(unsigned char *)0x8102B5 = 0;
        }
        func_001831F0(2);
        break;
    case 2:
        if (func_001BA1F0(self) != 0) {
            self[5] = 3;
        }
        *(short *)(self + 0x28) += 1;
        if (*(short *)(self + 0x28) >= 0x1F4) {
            if (*(short *)(self + 0x28) == 0x1F4) {
                *(unsigned char *)0x8102B5 = self[7];
            }
            func_001831F0(2);
        }
        break;
    case 3:
        if (D_008102B5 == 0) {
            self[5] = 4;
            func_001BA1A0(talk, D_overlay_AREA19_0082C690);
        } else {
            func_001831F0(2);
        }
        break;
    case 4:
        if (func_001BA1F0(self) != 0) {
            *(unsigned char *)0x8107F6 = 1;
            self[5] = 0;
        }
        break;
    }
}

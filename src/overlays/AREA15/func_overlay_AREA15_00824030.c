// NEARMISS func_overlay_AREA15_00824030 (95.13%, mwcc 2.3.3; linked from its splat .s)
// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA15 overlay, runtime 0x00824070 (splat/link name 00824030; overlay code is
// linked 0x40 below where it runs), 0x1D0 bytes.
// Role (read from the instructions): state 0: state 3 unless
//  func_001BA1C0(self, 0x23) is set and both 0x28 and 0x29 are clear; +0xD
//  0x50 -> func_001B10B0(.., 0x52), func_001C63E0(self, 9), +0x58 =
//  D_0028A5DC; 0x64 -> func_001B10B0(.., 0x65), func_001C63E0(self, 0), +0x58
//  = D_0028A6E8; func_001BA8E0(self, +0xD), state 1, +0 = 1, +0x30 = 0x8288F0.
//  State 1 by D_008107FC: 0 -> 0x824240 (only for +0xD 0x64, else return), 1
//  -> 0x824350 for +0xD 0x50, other -> 0x8243E0; then func_001C68C0, +1 = 1
//  and the +0x4C method. States 2/3 func_001BA540 and func_001AFC10.
// Divergence: the original fills the state-dispatch and first-check delay
//  slots by moving the successor's first instruction (the D_008107FC address
//  load, the 0x23 and 0x28 argument loads, the 0x52 argument) and leaves no
//  dead copy behind; mwcc 2.3.3 (and 2.4 / 991202) copy them and keep the dead
//  originals (idiom-13b), or leave the slot empty. Sibling functions with the
//  same shape (0x8235A0, 0x823850, 0x824560) do show the dead copies in the
//  original and match. return / goto / break exits, a switch local, flags
//  -O3/-O4,s and the other compilers were tried. The audit
//  (build/a15c/audit.py) finds only the two dead copies in the multiset
//  difference.
extern unsigned char D_008107FC;
extern int D_0028A5DC;
extern int D_0028A6E8;
extern char D_overlay_AREA15_008288F0[];
extern int func_001BA1C0(unsigned char *self, int idx);
extern void func_001B10B0(unsigned char *self, int a, int b);
extern void func_001C63E0(unsigned char *self, int a);
extern void func_001BA8E0(unsigned char *self, int a);
extern void func_001C68C0(unsigned char *self);
extern void func_001BA540(unsigned char *self);
extern void func_001AFC10(unsigned char *self);
extern void func_overlay_AREA15_00824240(unsigned char *self);
extern void func_overlay_AREA15_00824350(unsigned char *self);
extern void func_overlay_AREA15_008243E0(unsigned char *self);

void func_overlay_AREA15_00824030(unsigned char *self) {
    int id;
    switch (self[4]) {
    case 0:
        if (func_001BA1C0(self, 0x23) == 0) {
            self[4] = 3;
            break;
        }
        if (func_001BA1C0(self, 0x28) != 0 || func_001BA1C0(self, 0x29) != 0) {
            self[4] = 3;
            break;
        }
        id = self[0xD];
        if (id == 0x50) {
            func_001B10B0(self, (unsigned char)id, 0x52);
            func_001C63E0(self, 9);
            *(int *)(self + 0x58) = D_0028A5DC;
        }
        id = self[0xD];
        if (id == 0x64) {
            func_001B10B0(self, (unsigned char)id, 0x65);
            func_001C63E0(self, 0);
            *(int *)(self + 0x58) = D_0028A6E8;
        }
        func_001BA8E0(self, self[0xD]);
        self[4] = 1;
        self[0] = 1;
        *(char **)(self + 0x30) = D_overlay_AREA15_008288F0;
        break;
    case 1:
        switch (D_008107FC) {
        case 0:
            if (self[0xD] != 0x64) {
                return;
            }
            func_overlay_AREA15_00824240(self);
            break;
        case 1:
            if (self[0xD] == 0x50) {
                func_overlay_AREA15_00824350(self);
            }
            break;
        default:
            func_overlay_AREA15_008243E0(self);
            break;
        }
        func_001C68C0(self);
        self[1] = 1;
        (*(void (**)(unsigned char *))(self + 0x4C))(self);
        break;
    case 2:
    case 3:
        func_001BA540(self);
        func_001AFC10(self);
        break;
    }
}

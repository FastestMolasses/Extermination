// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 16
//
// Entity state-machine step, dispatched on the mode byte at arg0+6 (0..3):
//  case 0: advance mode; clear byte 7; func_001749A0(self,0x182,0,8.0f); zero the
//          small-data global word D_00275B00[2] (gp-relative).
//  case 1: if flag bit 0x8000 of word arg0+0x200 is clear, advance mode and call
//          func_0016BAE0(self, 0)  (self forwarded in $a0, the 0 arg in $a1).
//  case 2: if flag bit 0x1000 of arg0+0x200 is set, advance mode, stamp byte
//          arg0+0x1F0 = 0x27, and func_001749A0(self,0x180,0,1.0f).
//  case 3: if func_001607D0() returns 0, set bytes 4=1, 5=0x14, 6=0, 0x1F0=0x26.
// func_001749A0's 4th arg is a float passed in $f12 (does not consume a GPR slot,
// so self stays in $a0). D_00275B00 is gp-relative small data — needs the symbol
// inside the sdata threshold, hence -sdatathreshold 16 (default 8 excludes the
// 12-byte object and emits an absolute lui/lo store instead of the gp_rel store).
//
// mwcc 2.3.3 byte-matches; the pinned 991202 build caps at 92.5% (clean-store
// delay-slot nop, wall #13). Verified objdiff 100.0% vs build/expected.
extern int func_001607D0(void);
extern void func_0016BAE0(int, int);
extern void func_001749A0(int, int, int, float);
extern int D_00275B00[3];

void func_0016B790(int arg0) {
    unsigned char mode;

    mode = *(unsigned char *)(arg0 + 6);
    switch (mode) {
    case 0:
        *(unsigned char *)(arg0 + 6) = mode + 1;
        *(char *)(arg0 + 7) = 0;
        func_001749A0(arg0, 0x182, 0, 8.0f);
        D_00275B00[2] = 0;
        break;
    case 1:
        if (!(*(int *)(arg0 + 0x200) & 0x8000)) {
            *(unsigned char *)(arg0 + 6) = mode + 1;
            func_0016BAE0(arg0, 0);
        }
        break;
    case 2:
        if (*(int *)(arg0 + 0x200) & 0x1000) {
            *(unsigned char *)(arg0 + 6) = mode + 1;
            *(char *)(arg0 + 0x1F0) = 0x27;
            func_001749A0(arg0, 0x180, 0, 1.0f);
        }
        break;
    case 3:
        if (func_001607D0() == 0) {
            *(char *)(arg0 + 4) = 1;
            *(char *)(arg0 + 5) = 0x14;
            *(unsigned char *)(arg0 + 6) = 0;
            *(char *)(arg0 + 0x1F0) = 0x26;
        }
        break;
    }
}

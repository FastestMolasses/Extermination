// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
//
// Sequencer that drives two helpers (func_0020E3A0 returns a handle/id, fed
// straight into func_0020E1E0) based on a global mode byte D_00810CA4 and a
// sub-state byte D_00810CA6. Issues the base pair (-1, 0x10), then branches:
// mode 2 -> 0xC; mode 0 -> 0xA + CA6; otherwise -2, then mode 1 -> 0xB + CA6
// else CA5 + CA6 + CA7; finally if CA6 == 4, issue 0x15.
//
// Built with mwcc 2.3.3 (mwcps2-2.3.3-000906): under the pinned 991202 the
// residual is the delay-slot-fill wall (991202 fills the bne/jal delay slots
// that CW leaves as nop), stalling at 87.07%. 2.3.3 leaves those nops -> 100%.
extern int func_0020E1E0(int);
extern int func_0020E3A0(int);
extern unsigned char D_00810CA4;
extern unsigned char D_00810CA5;
extern unsigned char D_00810CA6;
extern unsigned char D_00810CA7;

void func_0020E250(void) {
    func_0020E1E0(func_0020E3A0(-1));
    func_0020E1E0(func_0020E3A0(0x10));
    if (D_00810CA4 == 2) {
        func_0020E1E0(func_0020E3A0(0xC));
        return;
    }
    if (D_00810CA4 == 0) {
        func_0020E1E0(func_0020E3A0(0xA));
        func_0020E1E0(func_0020E3A0(D_00810CA6));
        return;
    }
    func_0020E1E0(func_0020E3A0(-2));
    if (D_00810CA4 == 1) {
        func_0020E1E0(func_0020E3A0(0xB));
        func_0020E1E0(func_0020E3A0(D_00810CA6));
    } else {
        func_0020E1E0(func_0020E3A0(D_00810CA5));
        func_0020E1E0(func_0020E3A0(D_00810CA6));
        func_0020E1E0(func_0020E3A0(D_00810CA7));
    }
    if (D_00810CA6 == 4) {
        func_0020E1E0(func_0020E3A0(0x15));
    }
}

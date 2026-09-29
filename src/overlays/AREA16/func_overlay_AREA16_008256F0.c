// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x00825730 (splat/link name 008256F0; overlay code is
// linked 0x40 below where it runs), 0xC4 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// The slot holds two functions (mwcc emits one .text per function; each was
//  compared with the original separately and both are byte-identical) and
//  covers the splat pieces 008256F0, 00825730, 00825780; fill_overlay.py
//  merges the two .text sections and absorbs the later pieces.
// Role: two functions in one slot. 0x825730(self): releases the +0x2E0 handle
//  (func_001D80B0) and sets it to -1. 0x825780(self, idx) (link name
//  00825740): when +0x2E0 is valid and func_001D8060 returns a record, moves
//  it to the 0x825670 point (record + 0x10), else sets +0x2E0 = -1.
extern void func_001D80B0(int h);
extern unsigned char *func_001D8060(int h);
extern void func_overlay_AREA16_00825670(unsigned char *self, void *out, int idx);

void func_overlay_AREA16_008256F0(unsigned char *self) {
    int *h = (int *)(self + 0x1F0) + 0x3C;
    if (*(int *)(self + 0x2E0) != -1) {
        func_001D80B0(*(int *)(self + 0x2E0));
        *h = -1;
    }
}

void func_overlay_AREA16_00825740(unsigned char *self, int idx) {
    int *h = (int *)(self + 0x1F0) + 0x3C;
    unsigned char *p;
    if (*(int *)(self + 0x2E0) != -1) {
        p = func_001D8060(*(int *)(self + 0x2E0));
        if (p == 0) {
            *h = -1;
        } else {
            func_overlay_AREA16_00825670(self, p + 0x10, idx);
        }
    }
}

// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA16 overlay, runtime 0x00825670 (splat/link name 00825630; overlay code is
// linked 0x40 below where it runs), 0xC0 bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA16; lane A15C).
// The slot holds two functions (mwcc emits one .text per function; each was
//  compared with the original separately and both are byte-identical) and
//  covers the splat pieces 00825630, 00825660, 00825670, 008256E0;
//  fill_overlay.py merges the two .text sections and absorbs the later pieces.
// Role: two functions in one slot. 0x825670(self, out, idx): transforms
//  0x82C480[idx] by the +0x12C (idx 1) or +0x110 object's +0x90 matrix into
//  out (func_001026A0). 0x8256E0(self, idx) (the link name of that address is
//  008256A0): builds that point on the stack and stores func_001D7FA0(point,
//  0x82C4A0, 2, 1.0, 0.0) in +0x2E0.
extern float D_overlay_AREA16_0082C480[][4];
extern char D_overlay_AREA16_0082C4A0[];
extern void func_001026A0(void *dst, void *m, void *v);
extern int func_001D7FA0(void *v, void *tbl, int n, float f12, float f13);
extern void func_overlay_AREA16_00825670(unsigned char *self, void *out, int idx);

void func_overlay_AREA16_00825630(unsigned char *self, void *out, int idx) {
    if (idx == 1) {
        func_001026A0(out, *(unsigned char **)(self + 0x12C) + 0x90,
                      D_overlay_AREA16_0082C480[idx]);
    } else {
        func_001026A0(out, *(unsigned char **)(self + 0x110) + 0x90,
                      D_overlay_AREA16_0082C480[idx]);
    }
}

void func_overlay_AREA16_008256A0(unsigned char *self, int idx) {
    float v[4];
    func_overlay_AREA16_00825670(self, v, idx);
    *(int *)(self + 0x2E0) = func_001D7FA0(v, D_overlay_AREA16_0082C4A0, 2, 1.0f, 0.0f);
}

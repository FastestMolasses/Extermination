// COMPILER: mwcc233
// CFLAGS: -O4,p -sdatathreshold 0
// AREA21 overlay, runtime 0x00828E80 (splat/link name 00828E40; overlay code
//  is linked 0x40 below where it runs), 0xAC bytes.
// Byte-identical (tools/overlay/overlay_match.py check AREA21; lane A03C).
// Covers the splat pieces 00828E40, 00828E80 (the later piece is absorbed at
//  link time, tools/overlay/fill_overlay.py).
// Role: called from 0x828700 with (self, work, which): spawns a class 8
//  object with +3 = 6 and behaviour 0x828F30, at work +0x10 (which 0) or
//  +0x20, direction self +0x70; returns 1, or 0 when none was free.
extern char D_overlay_AREA21_00828F30[];
extern unsigned char *func_001AFA90(int cls);
extern void func_00102948(void *dst, void *src);

int func_overlay_AREA21_00828E40(unsigned char *src, unsigned char *rec, int which) {
    unsigned char *o = func_001AFA90(8);
    if (o != 0) {
        o[3] = 6;
        *(char **)(o + 0x10) = D_overlay_AREA21_00828F30;
        if (which == 0) {
            *(float *)(o + 0xB0) = *(float *)(rec + 0x10);
            *(float *)(o + 0xB4) = *(float *)(rec + 0x14);
            *(float *)(o + 0xB8) = *(float *)(rec + 0x18);
        } else {
            *(float *)(o + 0xB0) = *(float *)(rec + 0x20);
            *(float *)(o + 0xB4) = *(float *)(rec + 0x24);
            *(float *)(o + 0xB8) = *(float *)(rec + 0x28);
        }
        func_00102948(o + 0xC0, src + 0x70);
        return 1;
    }
    return 0;
}
